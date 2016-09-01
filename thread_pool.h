#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <list>
#include <exception>

#include "locker.h"

template<typename T>
class ThreadPool {
public:
    ThreadPool(int thread_num = 8, int max_requests = 10000);

    ~ThreadPool();

    bool append(T *request);


private:
    static void *worker(void *param);

    void run();

private:
    int _thread_num;
    int _max_requests;
    pthread_t *_threads;
    std::list<T *> _work_queue;
    Locker _queue_locker;
    Sem _queue_sem;
    bool _stop;
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_num, int max_requests) :
        _thread_num(thread_num), _max_requests(max_requests), _stop(false), _threads(NULL) {
    if (_thread_num <= 0 || _max_requests <= 0) {
        throw std::exception();
    }

    _threads = new pthread_t[_thread_num];
    if (!_threads) {
        throw std::exception();
    }

    for (int i = 0; i < _thread_num; i++) {
        printf("create the %dth thread\n", i);
        if (pthread_create(_threads + i, NULL, worker, this) != 0) {
            delete[]_threads;
            throw std::exception();
        }

        if (pthread_detach(_threads[i])) {
            delete[]_threads;
            throw std::exception();
        }
    }

}

template<typename T>
ThreadPool<T>::~ThreadPool() {
    delete[]_threads;
    _top = true;
}

template<typename T>
bool ThreadPool<T>::append(T *request) {
    _queue_locker.lock();
    if (_work_queue.size() > _max_requests) {
        _queue_locker.unlock();
        return false;
    }
    _work_queue.push_back(request);
    _queue_locker.unlock();
    _queue_sem.post();
    return true;
}

template<typename T>
void ThreadPool<T>::worker(void *param) {
    ThreadPool *pool = (ThreadPool *) param;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run() {
    while (!_stop) {
        _queue_sem.wait();
        _queue_locker.lock();
        if (_work_queue.empty()) {
            _queue_locker.unlock();
            continue;
        }
        T *request = _work_queue.front();
        _work_queue.pop_front();
        _queue_locker.unlock();
        if (!request) {
            continue;
        }
        request.process();
    }
}




