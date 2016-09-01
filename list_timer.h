#ifndef LIST_TIMER
#define LIST_TIMER

#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 64

class UtilTimer;

typedef struct {
    struct sockaddr_in cliaddr;
    int clifd;
    char buf[BUFFER_SIZE];
    UtilTimer *timer;
} cli_data_t;


class SortTimerList;

class UtilTimer {
    public:
        UtilTimer():prev(NULL), next(NULL) {}


    public:
        UtilTimer *prev;
        UtilTimer *next;

    public:
        time_t expire;
        void (*cb_func)(cli_data_t *);
        cli_data_t *_cli_data;
    public:
        friend SortTimerList;
};

class SortTimerList {
    public:
        SortTimerList() : _head(NULL), _tail(NULL) {}
        ~SortTimerList();
    public:
        void add_timer(UtilTimer *timer);
        void adjust_timer(UtilTimer *timer);
        void del_timer(UtilTimer *timer);
        void tick();
    private:
        void add_timer(UtilTimer *timer, UtilTimer *list_head_);
    private:
        UtilTimer *_head;
        UtilTimer *_tail;
};



#endif /* ifndef LIST_TIMER*/
