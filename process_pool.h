#ifndef __PROCESS_POOL_H__
#define __PROCESS_POOL_H__

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>


class Process {
public:
    Process() : pid(-1) {}

public:
    pid_t pid;
    int pipefd[2];
};

template<typename T>
class ProcessPool {
public:
    static ProcessPool<T> *create(int listenfd, int process_number = 8) {
        if (!_instance) {
            _instance = new ProcessPool<T>(listenfd, process_number);
        }
        return _instance;
    }

    ~ProcessPool() {
        delete[] _sub_processes;
    }

    void run();

private:
    void setup_sig_pipe();

    void run_parent();

    void run_child();

private:
    ProcessPool(int listenfd, int process_number = 8);

private:
    static const int MAX_PROCESS_NUMBER = 16;
    static const int USER_PER_PROCESS = 65536;
    static const int MAX_EVENT_NUBMER = 10000;
    int _process_number;
    int _idx;
    int _epollfd;
    int _listenfd;
    int _stop;
    Process *_sub_processes;
    static ProcessPool<T> *_instance;
};

template<typename T>
ProcessPool<T> *ProcessPool<T>::_instance = NULL;

static int sig_pipefd[2];

static int setnonbloking(int fd) {
    int oldflag = fcntl(fd, F_GETFL);
    int newflag = oldflag | O_NONBLOCK;
    fcntl(fd, F_SETFL, newflag);
    return oldflag;
}

static void addfd(int epfd, int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, &event);
    setnonbloking(fd);
}

static void removefd(int epfd, int fd) {
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
}

static void sig_handler(int sig) {
    int save_errno = errno;
    int msg = sig;
    send(sig_pipefd[1], (char *) &msg, 1, 0);
    errno = save_errno;
}

static void addsig(int sig, void (*handler)(int), bool restart = true) {
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

template<typename T>
ProcessPool<T>::ProcessPool(int listenfd, int process_number)
        : _listenfd(listenfd), _process_number(process_number), _idx(-1), _stop(false) {
    assert((_process_number > 0) && (process_number <= MAX_PROCESS_NUMBER));

    _sub_processes = new Process[_process_number];
    assert(_sub_processes);

    for (int i=0; i< _process_number; i++) {
        int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, _sub_processes[i].pipefd);
        assert(ret == 0);
        _sub_processes[i].pid = fork();
        assert(_sub_processes[i].pid >= 0);
        if (_sub_processes[i].pid > 0) {
            close(_sub_processes[i].pipefd[1]);
            continue;
        } else {
            close(_sub_processes[i].pipefd[0]);
            _idx = i;
            break;
        }
    }
}

template <typename T>
void ProcessPool<T>::setup_sig_pipe() {
    _epollfd = epoll_create(5);
    assert(_epollfd != -1);

    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipefd);
    assert(ret != -1);
    setnonbloking(sig_pipefd[1]);
    addfd(_epollfd, sig_pipefd[0]);

    addsig(SIGCHLD, sig_handler);
    addsig(SIGTERM, sig_handler);
    addsig(SIGINT, sig_handler);
    addsig(SIGPIPE, SIG_IGN);
}

template <typename T>
void ProcessPool<T>::run() {
    if (_idx == -1) {
        run_child();
        return;
    }
    run_parent();
}


template <typename T>
void ProcessPool<T>::run_child() {
    setup_sig_pipe();
    int pipefd = _sub_processes[_idx].pipefd[1];
    addfd(_epollfd, pipefd);
    epoll_event events[MAX_EVENT_NUBMER];
    T *users = new T[USER_PER_PROCESS];
    assert(users);
    int number = 0;
    int ret = -1;

    while (!_stop) {
        number = epoll_wait(_epollfd, events, MAX_EVENT_NUBMER, -1);
        if ((number < 0) && (errno != EINTR)) {

        }
    }
}


#endif
