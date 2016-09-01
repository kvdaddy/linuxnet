#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 1024

int setnonbloking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}


int unblock_connect(const char *ip, int port, int time) {
    int ret = 0;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons(port);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int fdopt = setnonbloking(sockfd);
    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == 0) {
        //connect sucessful
        printf("connect with server immediate\n");
        fcntl(sockfd, F_SETFL, fdopt);
        return sockfd;
    } else if (errno != EINPROGRESS) {
        printf("unblock connect not support\n");
        return -1;
    }

    fd_set readfds;
    fd_set writefds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    ret = select(sockfd + 1, NULL, &writefds, NULL, &timeout);

    if (ret <= 0) {
        printf("connection time out \n");
        close(sockfd);
        return -1;
    }

    if (!FD_ISSET(sockfd, &writefds)) {
        printf("no events on sockfd found\n");
        close(sockfd);
        return -1;
    }

    int error = 0;
    socklen_t len = sizeof(error);

    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        printf("get socket option failed\n");
        close(sockfd);
        return -1;
    }

    if (error != 0) {
        printf("connection failed after select with error: %d\n", error);
        close(sockfd);
        return -1;
    }

    printf("connection ready after select with the socket:%d\n", sockfd);
    fcntl(sockfd, F_SETFL, fdopt);
    return sockfd;

}

int main(int argc, char *argv[]) {
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int sockfd = unblock_connect(ip, port, 10);
    if (sockfd < 0) {
        return 1;
    }
    close(sockfd);
    return 0;
}
