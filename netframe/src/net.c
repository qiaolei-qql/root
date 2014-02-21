#include "net.h"

list_head_ptr(conn_list) g_conn_list_head = NULL;
uint32_t conn_list_maxnum = UINT16_MAX;
uint32_t conn_list_size;

static socket_event_s g_socket_events[UINT16_MAX];
static int g_max_event_num = UINT16_MAX;
static int g_maxfd = 0;

int create_ev_loop(ev_loop_s* ev_loop_s_ptr)
{
 
    ev_loop_s_ptr->ev_loop_ptr = ev_loop_new(EVBACKEND_EPOLL);
    assert(ev_loop_s_ptr->ev_loop_ptr != NULL);
    ev_loop_s_ptr->socket_event_ptr = g_socket_events;
    return 0;
}
int delete_ev_loop(ev_loop_s* ev_loop_s_ptr)
{
    g_maxfd = 0;
    if(ev_loop_s_ptr)
    {
        ev_loop_destroy(ev_loop_s_ptr->ev_loop_ptr);
    }
}
int create_ev_event(ev_loop_s* ev_loop_s_ptr, int fd, int mask,ev_sock_cb* cb_proc, void* client_data)
{
    if (fd > g_max_event_num){
        perror("fd > max_event_num");
        return -1;
    }
    if (fd > g_maxfd){
        g_maxfd = fd;
    }
    socket_event_s* socket_event_ptr = &ev_loop_s_ptr->socket_event_ptr[fd];
    socket_event_ptr->mask = mask;
    socket_event_ptr->fd = fd;
    socket_event_ptr->client_data =  client_data;
    ev_io_init(&socket_event_ptr->wather, cb_proc, fd, mask);
    ev_io_start(ev_loop_s_ptr->ev_loop_ptr, &socket_event_ptr->wather);
    return 0;
}

int delete_ev_event(ev_loop_s* ev_loop_s_ptr, int fd)
{
    if (fd > g_max_event_num){
        return -1;
    }
    socket_event_s* socket_event_ptr = &ev_loop_s_ptr->socket_event_ptr[fd];
    socket_event_ptr->mask = 0;
    ev_io_stop(ev_loop_s_ptr->ev_loop_ptr,&socket_event_ptr->wather);

    if (fd == g_maxfd) 
    {
        while(ev_loop_s_ptr->socket_event_ptr[g_maxfd].mask == 0)
        {
            g_maxfd--;
        }
    }
    return 0;
}

/*
 * conn proc api
 */
int conn_list_init()
{
    if (NULL == g_conn_list_head) {
        list_new(conn_list, conn_list_head);
        assert(conn_list_head!= NULL);
        g_conn_list_head = conn_list_head;
    }
    return 0;
}
list_item_ptr(conn_list) create_client(enum conn_type_e conn_type, enum conn_stat_e conn_stat, int fd)
{
    if (conn_list_size > conn_list_maxnum) {
        // log
        return NULL;
    }
    list_item_new(conn_list,conn_ptr);   
    assert(conn_ptr != NULL);
    conn_ptr->conn_type = conn_type;
    conn_ptr->conn_stat = conn_stat;
    conn_ptr->fd = fd;
    conn_ptr->event_mask = EV_READ;
    
    conn_ptr->rbuf = NULL;
    conn_ptr->rbytes = 0;
    conn_ptr->rsize = 1024;

    conn_ptr->wbuf = NULL;
    conn_ptr->wbytes = 0;
    conn_ptr->wsize = 1024;

    conn_ptr->thread_id= 0;
    
    list_lock(g_conn_list_head);
    conn_list_size++;
    list_push(g_conn_list_head,conn_ptr);
    list_unlock(g_conn_list_head);

    return conn_ptr;
}

