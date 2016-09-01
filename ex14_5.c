//
// Created by zhanglm on 16-8-31.
//


#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define HANDLE_ERROR(en, msg) \
    do { errno = en; perror(msg); exit(-1);} while(0)


static void *sig_thread(void *param) {
    sigset_t *set = (sigset_t *)param;
    int s, sig;
    for (;;) {
        s = sigwait(set, &sig);
        if (s != 0) {
            HANDLE_ERROR(s, "sigwait");
        }
        printf("Signal handling thread got signal %d\n", sig);
    }
}

int main(int argc, char *argv[]) {
    pthread_t thread;
    sigset_t set;
    int s;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0) {
        HANDLE_ERROR(s, "pthread_sigmask");
    }

    s = pthread_create(&thread, NULL, sig_thread, &set);
    if (s != 0) {
        HANDLE_ERROR(s, "pthread_create");
    }

    pthread_join(thread, NULL);
}
