#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>


#define MAX_EVENTS_NUMBER 1024
#define BUFFER_SIZE 10

int setnonbloking(int fd) {
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(fd, F_SETFL, newflag);
    return oldflag;
}

void addfd(int epfd, int fd, bool enable_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonbloking(fd);
}

void et(struct epoll_event *events, int number, int epfd, int listenfd) {
    char buf[BUFFER_SIZE];
    for (int i=0;i<number;i++) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            struct sockaddr_in cliaddr;
            socklen_t cliaddr_len = sizeof(cliaddr);
            int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
            addfd(epfd, connfd, true);
        } else if (events[i].events & EPOLLIN) {
            printf("event trigger once\n");
            while(1) {
                memset(buf, 0, BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE -1, 0);
                if (ret < 0) {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                } else if (ret == 0) {
                    close(sockfd);
                } else {
                    printf("get %d bytes of content: %s\n", ret, buf);
                }
            }
        } else {
            printf("something else happened \n");
        }
    }
}


int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);

    int ret = 0;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ret = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    ret = listen(listenfd, 5);

    struct epoll_event events[MAX_EVENTS_NUMBER];
    int epfd = epoll_create(5);

    addfd(epfd, listenfd, true);

    while(1) {
        int ret = epoll_wait(epfd, events, MAX_EVENTS_NUMBER, -1);
        if (ret < 0) {
            printf("epoll failure\n");
            break;
        }
        et(events, ret, epfd, listenfd);
    }

    close(listenfd);
    return 0;

}


