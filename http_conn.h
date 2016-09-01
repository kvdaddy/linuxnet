#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#define MAX_FILENAME_LEN 256
#define MAX_READ_BUF_SIZE 2048
#define MAX_WRITE_BUF_SIZE 2048;

class HttpConn {
public:
    HttpConn();
    ~HttpConn();

public:
    void init(int sockfd, const sockaddr_in &addr);
    void close_conn(bool real_close=true);
    void process;
    bool read();
    bool write();

private:
    void init();
    HTTP_CODE process_read();
    bool process_write(HTTP_CODE ret);
private:
    HTTP_CODE parse_reqeust_line(const char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();
    inline char *get_line() {return _read_buf + _start_line;}
    LINE_STATUS parse_line();

public:
    typedef enum {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATCH
    } METHOD;

    typedef enum {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    } CHECK_STATE;

    typedef enum {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    } HTTP_CODE;

    typedef enum {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    } LINE_STATUS;



};

#endif