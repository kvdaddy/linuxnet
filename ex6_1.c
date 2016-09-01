#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("usage: %s port", argv[0]);
        return 1;
    }

    struct sockaddr_in svraddr;
    memset(&svraddr, 0, sizeof(svraddr));

    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(atoi(argv[1]));
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int ret = bind(sock, (struct sockaddr *)&svraddr, sizeof(svraddr));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    int connfd = accept(sock, (struct sockaddr *)&cliaddr, &cliaddr_len);
    if (connfd < 0) {
        printf("errno is: %d", errno);
    } else {
        close(STDOUT_FILENO);
        dup(connfd);
        printf("abcd\n");
        close(connfd);
    }
    close(sock);
    return 0;



}
