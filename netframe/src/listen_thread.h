/*
 * filename      : listen_thread.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2014-02-18 19:53
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _LISTEN_THREAD_H_
#define _LISTEN_THREAD_H_
#include "net.h"

typedef struct 
{
    ev_loop_s listen_ev_loop;
    pthread_t thread_id;
}listen_thread_s;

typedef struct
{
    char* listen_ip;
    int port;
    int fd;
}listen_socket_s;

static void* listen_thread_proc(void* arg);
static void accept_tcp_cb(struct ev_loop* loop, struct ev_io* io_w, int events);

/*
 * interface 
 */
void listen_thread_init();
int add_listen(char* ip, int port, enum socket_event_type_e socket_type);
#endif
