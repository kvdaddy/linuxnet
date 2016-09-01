#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip port recv_buf_size", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock > 0);

    int recvbuf = atoi(argv[3]);
    int len = sizeof(recvbuf);

    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf));
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recvbuf, (socklen_t *)&len);
    printf("the tcp receive buffer size after setting is %d\n", recvbuf);

    int ret = bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    int connfd = accept(sock, (struct sockaddr *)&cliaddr, &cliaddr_len);
    if (connfd < 0) {
        printf("errno is: %d\n", errno);
    } else {
        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', BUFFER_SIZE);
        int ret = 0;
        while((ret = recv(connfd, buffer, BUFFER_SIZE -1, 0)) > 0) { printf("recv %d\n", ret); }
        printf("recv finished!\n");
        close(connfd);
    }

    close(sock);

    return 0;
}
