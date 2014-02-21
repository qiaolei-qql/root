/*
 * filename      : thread_poll.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2014-02-13 19:51
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _THREAD_POLL_H_
#define _THREAD_POLL_H_

#include "net.h"

#define WORK_THREAD_NUM 8

typedef struct {
    pthread_t thread_id;
    ev_loop_s work_ev;
    int notify_read_fd;
    int notify_write_fd;
    list_head_ptr(conn_list) new_conns;
} LIBEV_WORK_THREAD;

static void* work_thread_proc(void* arg);
static void pipe_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events);
static void client_conn_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events);

void dispather_thread_init(int nwork_threads);
int dispather_conn(socket_event_s* socket_event_ptr);
#endif
