#include "list_timer.h"
#include <stdio.h>


SortTimerList::~SortTimerList() {
    UtilTimer *tmp = _head;
    while(tmp) {
        _head = tmp->next;
        delete tmp;
        tmp = _head;
    }
}

void SortTimerList::add_timer(UtilTimer *timer_) {
    if(!timer_) {
        return;
    }
    if (!_head) {
        _head = _tail = timer_;
        return;
    }

    if (timer_->expire < _head->expire) {
        timer_->next = _head;
        _head->prev = timer_;
        _head = timer_;
        return;
    }

    add_timer(timer_, _head);
}

void SortTimerList::adjust_timer(UtilTimer *timer_) {
    if (!timer_) {
        return;
    }

    UtilTimer *tmp = timer_->next;
    if (!tmp || (timer_->expire < tmp->expire)) {
        return;
    }

    if (timer_ == _head) {
        _head = _head->next;
        _head->prev = NULL;
        timer_->next = NULL;
        add_timer(timer_, _head);
    } else {
        timer_->prev->next = timer_->next;
        timer_->next->prev = timer_->prev;
        add_timer(timer_, timer_->next);
    }
}

void SortTimerList::del_timer(UtilTimer *timer_) {
    if (!timer_) {
        return;
    }

    if ((timer_ == _head) && (timer_ == _tail)) {
        delete timer_;
        _head = _tail = NULL;
        return;
    }

    if (timer_ == _head) {
        _head = _head->next;
        _head->prev = NULL;
        delete timer_;
        return;
    }

    if (timer_ == _tail) {
        _tail = _tail->prev;
        _tail->next = NULL;
        delete timer_;
        return;
    }

    timer_->prev->next = timer_->next;
    timer_->next->prev = timer_->prev;
    delete timer_;
}

void SortTimerList::tick() {
    if (!_head) {
        return;
    }

    printf("timer tick\n");
    time_t cur = time(NULL);
    UtilTimer *tmp = _head;
    while(tmp) {
        if (cur < tmp->expire) {
            break;
        }

        tmp->cb_func(tmp->_cli_data);
        _head = tmp->next;
        if (_head) {
            _head->prev = NULL;
        }
        delete tmp;
        tmp = _head;
    }
}

void SortTimerList::add_timer(UtilTimer *timer_, UtilTimer *list_head_) {
    UtilTimer *prev = list_head_;
    UtilTimer *tmp = prev->next;

    while(tmp) {
        if (timer_->expire < tmp->expire) {
            prev->next = timer_;
            timer_->next = tmp;
            tmp->prev = timer_;
            timer_->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    prev->next = timer_;
    timer_->prev = prev;
    timer_->next = NULL;
    _tail = timer_;
}




