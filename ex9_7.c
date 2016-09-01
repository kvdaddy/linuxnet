#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENTS_NUMBER 1024
#define TCP_BUFFER_LEN 512


int setnonbloking(int fd)  {
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(fd, F_SETFL, newflag);
    return oldflag;
}

void addfd(int epfd, int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    setnonbloking(fd);
}

int main(int argc, char *argv[])
{
    if (argc <= 2) {
        printf("usage: %s <port1> <port2>\n", argv[0]);
        return 1;
    }

    int port1 = atoi(argv[1]);
    int port2 = atoi(argv[2]);

    struct sockaddr_in addr1;
    bzero(&addr1, sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(port1);
    addr1.sin_addr.s_addr = htonl(INADDR_ANY);

    struct sockaddr_in addr2;
    bzero(&addr2, sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(port2);
    addr2.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd1 = socket(AF_INET, SOCK_STREAM, 0);
    int listenfd2 = socket(AF_INET, SOCK_STREAM, 0);

    bind(listenfd1, (struct sockaddr *)&addr1, sizeof(addr1));
    listen(listenfd1, 5);
    bind(listenfd2, (struct sockaddr *)&addr2, sizeof(addr2));
    listen(listenfd2, 5);

    int epollfd = epoll_create(5);
    struct epoll_event events[MAX_EVENTS_NUMBER];

    addfd(epollfd, listenfd1);
    addfd(epollfd, listenfd2);

    while(1) {
        int number = epoll_wait(epollfd, events, MAX_EVENTS_NUMBER, -1);
        if (number <0) {
            printf("epoll_wait failed\n");
            break;
        }

        for (int i=0;i<number;i++) {
            int peerfd = events[i].data.fd;
            if (peerfd == listenfd1 || peerfd == listenfd2) {
                struct sockaddr_in cliaddr;
                bzero(&cliaddr, sizeof(cliaddr));
                socklen_t cliaddr_len = sizeof(cliaddr);
                int connfd = accept(peerfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
                addfd(epollfd, connfd);
            } else if (events[i].events & EPOLLIN) {
                char buf[TCP_BUFFER_LEN];
                bzero(buf, TCP_BUFFER_LEN);
                int ret =recv(peerfd, buf, TCP_BUFFER_LEN -1, 0);
                if (ret < 0) {
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                        break;
                    }
                    close(peerfd);
                    break;
                } else if (ret == 0) {
                    close(peerfd);
                } else {
                    send(peerfd, buf, ret, 0);
                }
            } else {
                printf("something else happened\n");
            }

        }
    }

    close(listenfd1);
    close(listenfd2);
    return 0;

}



