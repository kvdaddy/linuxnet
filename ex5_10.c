#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip port send_buf_size\n", basename(argv[0]));
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
    assert(sock >= 0);

    int sendbuf = atoi(argv[3]);
    int len = sizeof(sendbuf);
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, (socklen_t *)&len);
    printf("the tcp send buffer size ofter setting is %d", sendbuf);

    if (connect(sock, (struct sockaddr *)&servaddr , sizeof(servaddr)) != -1) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 'a', BUFFER_SIZE);
        send(sock, buffer, BUFFER_SIZE, 0);
    }
    close(sock);
    return 0;
}
