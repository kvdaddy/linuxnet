#include "list_timer.h"
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMESLOT 5
static int pipefd[2];
static SortTimerList timer_list;
static int epollfd = 0;



int setnonbloking(int fd) {
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(fd, F_SETFL, newflag);
    return oldflag;
}

int addfd(int epfd, int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonbloking(fd);
}

void sig_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

void timer_handler() {
    timer_list.tick();
    alarm(TIMESLOT);
}

void cb_func(cli_data_t *user_data) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->clifd, 0);
    assert(user_data);
    close(user_data->clifd);
    printf("close fd %d\n", user_data->clifd);
}

int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    struct epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd);

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonbloking(pipefd[0]);
    addfd(epollfd, pipefd[0]);

    addsig(SIGALRM);
    addsig(SIGTERM);
    bool stop_server = false;

    cli_data_t *users = new cli_data_t[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);

    ret = listen(listenfd, 5);

    while(!stop_server) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((number < 0) && (errno != EINTR)) {
            printf("epoll failure\n");
            break;
        }
        for (int i=0;i<number;i++) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                struct sockaddr_in cliaddr;
                socklen_t cliaddr_len = sizeof(cliaddr);
                int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
                addfd(epollfd, connfd);
                users[connfd].cliaddr = cliaddr;
                users[connfd].clifd = connfd;

                UtilTimer *timer = new UtilTimer();
                timer->_cli_data = &users[connfd];
                timer->cb_func = cb_func;
                time_t cur = time(NULL);
                timer->expire = cur + 3*TIMESLOT;
                users[connfd].timer = timer;
                timer_list.add_timer(timer);
            } else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)) {
                int sig;
                char signals[1024];
                ret = recv(sockfd, signals, sizeof(signals), 0);
                if (ret == -1) {
                    continue;
                } else if(ret == 0) {
                    continue;
                } else {
                    for (int i=0;i<ret;i++){
                        switch (signals[i]) {
                            case SIGALRM:
                                timeout = true;
                                break;
                            case SIGTERM:
                                stop_server = true;
                        }

                    }
                }
            } else if (events[i].events & EPOLLIN) {
                memset(users[sockfd].buf, 0, BUFFER_SIZE);
                ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE -1, 0);
                printf("get %d bytes of client dta %s from %d\n", ret, users[sockfd].buf, sockfd);
                UtilTimer *timer = users[sockfd].timer;
                if (ret < 0) {
                    if (errno != EAGAIN) {
                        cb_func(&users[sockfd]);
                        if (timer) {
                            timer_list.del_timer(timer);
                        }
                    }
                } else if (ret == 0) {
                    cb_func(&users[sockfd]);
                    if (timer) {
                        timer_list.del_timer(timer);
                    }
                } else {
                    if (timer) {
                        time_t cur = time(NULL);
                        timer->expire = cur + 3 * TIMESLOT;
                        printf("adjust timer once\n");
                        timer_list.adjust_timer(timer);
                    }
                }
            } else {
                printf("something strange happen\n");
            }
        }

        if (timeout) {
            timer_handler();
            timeout = false;
        }
    }

    close(listenfd);
    close(pipefd[0]);
    close(pipefd[1]);
    delete []users;
    return 0;

}

