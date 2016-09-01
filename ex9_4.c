#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#define MAX_EVENTS_NUBMER 1024
#define MAX_BUFFER_SIZE 1024

typedef struct {
    int epollfd;
    int sockfd;
} fds;

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epfd, int fd, bool oneshot) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if (oneshot) {
        event.events |= EPOLLONESHOT;
    }

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void reset_oneshot(int epfd, int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}


void *worker(void *arg) {
    int sockfd = ((fds *)arg)->sockfd;
    int epfd = ((fds*)arg)->epollfd;
    printf("start new thread to receive data on fd %d", sockfd);
    char buf[MAX_BUFFER_SIZE];
    memset(buf, 0, sizeof(buf));
    while(1) {
        int ret = recv(sockfd, buf, MAX_BUFFER_SIZE - 1, 0);
        if (ret == 0) {
            close(sockfd);
            printf("foreign peer closed the connection\n");
            break;
        } else if(ret < 0) {
            if (errno == EAGAIN) {
                reset_oneshot(epfd, sockfd);
                printf("reader later\n");
                break;
            }
        } else {
            printf("get content: %s\n", buf);
            sleep(5);
        }
    }
    printf("end thread receiving data on fd %d\n", sockfd);
    return 0;
}


int main(int argc, char * argv[]) {
    if (argc <= 1) {
        printf("usage: %s port", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int ret = 0;
    struct sockaddr_in svraddr;
    memset(&svraddr, 0, sizeof(svraddr));
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(port);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ret = bind(listenfd, (struct sockaddr *)&svraddr, sizeof(svraddr));
    ret = listen(listenfd, 5);

    struct epoll_event events[MAX_EVENTS_NUBMER];
    int epollfd = epoll_create(1);
    addfd(epollfd, listenfd, false);

    while(1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENTS_NUBMER, -1);
        if (ret < 0) {
            printf("epoll failure");
        }

        for (int i=0; i<ret; i++) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenfd) {
                printf("new connection\n");
                struct sockaddr_in cliaddr;
                socklen_t cliaddr_len = sizeof(cliaddr);
                int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
                addfd(epollfd, connfd, true);
            } else if (events[i].events & EPOLLIN) {
                pthread_t t;
                fds fds_for_new_worker;
                fds_for_new_worker.sockfd = sockfd;
                fds_for_new_worker.epollfd = epollfd;
                pthread_create(&t, NULL, worker, (void*)&fds_for_new_worker);
                printf("new worker stared\n");
            } else {
                printf("something else happened\n");
            }
        }
    }

    close(listenfd);
    return 0;
}
