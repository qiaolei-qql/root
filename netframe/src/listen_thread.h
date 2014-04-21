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
#include "queue.h"
/*
list_def(listen_list);
struct list_item_name(listen_list) {
    char listen_ip[16];
    int port;
    int fd;
    enum conn_type_e conn_type;
    list_next_ptr(listen_list);
};
*/
typedef struct 
{
    ev_loop_s listen_ev_loop;
    pthread_t thread_id;
    list_head_ptr(listen_list) listen_list_head;
}listen_thread_s;


//static void* listen_thread_proc(void* arg);
//static void accept_tcp_cb(struct ev_loop* loop, struct ev_io* io_w, int events);

/*
 * interface 
 */
void listen_thread_init();
void listen_thread_run();
int add_listen(char* ip, int port, enum socket_event_type_e socket_type);
#endif
