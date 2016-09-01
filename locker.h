//
// Created by zhanglm on 16-8-31.
//

#ifndef LINUX_NETWORK_LOCKER_H
#define LINUX_NETWORK_LOCKER_H


class Sem {
public:
    Sem() {
        if (!sem_init(&_sem, 0, 0) != 0) {
            throw std::exception;
        }
    }

    ~Sem() {
        sem_destroy(_sem);
    }

public:
    bool wait() {
        return sem_wait(&_sem) == 0;
    }

    bool post() {
        return sem_post(&_sem) == 0;
    }

private:
    sem_t _sem;
};

class Locker {
public:
    Locker() {
        if (pthread_mutex_init(&_mutex, NULL) != 0) {
            throw std::exception;
        }
    }
    ~Locker() {
        pthread_mutex_destroy(&_mutex);
    }

public:
    bool lock() {
        return pthread_mutex_lock(&_mutex) == 0;
    }

    bool unlock() {
        return pthread_mutex_unlock(&_mutex) == 0;
    }
private:
    pthread_mutex_t _mutex;
};

class Cond {
public:
    Cond() {
        if (pthread_mutex_init(&_mutex, NULL) != 0) {
            throw std::exception;
        }
        if (pthread_cond_init(&_cond, NULL) != 0) {
            throw std::exception;
        }
    }

    ~Cond() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

public:
    bool wait() {
        int ret = 0;
        pthread_mutex_lock(_mutex);
        ret = pthread_cond_wait(&_cond, &_mutex);
        pthread_mutex_unlock(&_mutex);
        return ret == 0;
    }

    bool signal() {
        return pthread_cond_signal(&_cond) == 0;
    }

private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
};


#endif //LINUX_NETWORK_LOCKER_H
