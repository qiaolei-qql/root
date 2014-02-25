/*
 * filename      : net.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2014-02-19 10:46
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _NET_H_
#define _NET_H_

#include <ev.h>
#include "queue.h"
#include "base_header.h"

/*
 * conn
*/
enum conn_stat_e{
    CONN_LISTENING,
    CONN_ESTABLISHED,
    CONN_CLOSED
};
enum conn_type_e{
    CONN_CLIENT,
    CONN_SERVER
};


list_def(conn_list);
struct list_item_name(conn_list){
    int fd;
    char listen_ip[16];
    int port;
    enum conn_stat_e conn_stat;
    enum conn_type_e conn_type;

    int event_mask;
    int which;

    char* rbuf;
    int rbytes;
    int rsize;
    char* wbuf;
    int wbytes;
    int wsize;
    int thread_id;
    list_next_ptr(conn_list);
};
int conn_list_init();
list_item_ptr(conn_list) create_client(enum conn_type_e conn_type, enum conn_stat_e conn_stat, int fd);

/*
 * socket_event 
 */
enum socket_event_type_e {
    LISTEN_CLIENT,
    LISTEN_SERVER,
    SERVER_SOCKET,
    CLIENT_SOCKET,
    PIPE_EVENT
};
typedef struct {
    struct ev_io wather;
    struct ev_timer socket_timer;
    int mask;
    enum socket_event_type_e event_type;
    int fd;
    void* client_data;
}socket_event_s;


typedef struct {
    struct ev_loop* ev_loop_ptr;
    socket_event_s* socket_event_ptr;
    int stop;
} ev_loop_s;


typedef void ev_sock_cb(struct ev_loop* loop, struct ev_io* io_w, int events);

int create_ev_loop(ev_loop_s* ev_loop_s_ptr);
int delete_ev_loop(ev_loop_s* ev_loop_s_ptr);

int create_ev_event(ev_loop_s* ev_loop_s_ptr,int fd, int mask,ev_sock_cb* cb_proc, void* client_data);
int delete_ev_event(ev_loop_s* ev_loop_s_ptr,int fd);
#endif
