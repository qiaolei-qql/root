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
#include "message_handler.h"

#define WORK_THREAD_NUM 8


extern __thread  volatile uint64_t g_threadid;

typedef struct {
    pthread_t thread_id;
    ev_loop_s work_ev;
    int notify_read_fd;
    int notify_write_fd;
    list_head_ptr(msg_queue) thread_msgs;
} LIBEV_WORK_THREAD;

extern LIBEV_WORK_THREAD* g_work_threads;


//static void* work_thread_proc(void* arg);
//static void pipe_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events);

void client_conn_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events);


void work_thread_init(int nwork_threads);
int dispath_new_conn(conn_t* conn_ptr);
int post_message(int thread_id, message_handler* handler, void* arg1, void* arg2);

#endif
