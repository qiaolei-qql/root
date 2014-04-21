#include "net.h"


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

int create_ev_event(ev_loop_s* ev_loop_s_ptr,const int fd, int mask,ev_sock_cb* cb_proc, void* client_data)
{
    if (fd > g_max_event_num){
        perror("fd > max_event_num");
        return -1;
    }
    if (fd > g_maxfd){
        g_maxfd = fd;
    }
    log_debug("ev_loop_s_ptr: %p ",ev_loop_s_ptr);
    socket_event_s* socket_event_ptr = &ev_loop_s_ptr->socket_event_ptr[fd];
    if (socket_event_ptr->mask != 0){
        ev_io_stop(ev_loop_s_ptr->ev_loop_ptr, &socket_event_ptr->wather);
    }
    socket_event_ptr->mask |= mask;
    socket_event_ptr->fd = fd;
    log_debug("client_data: %p %p",socket_event_ptr->client_data ,client_data);
    socket_event_ptr->client_data =  client_data;
    ev_io_init(&socket_event_ptr->wather, cb_proc, fd, mask);
    ev_io_start(ev_loop_s_ptr->ev_loop_ptr, &socket_event_ptr->wather);
    return 0;
}

int delete_ev_event(ev_loop_s* ev_loop_s_ptr,const int fd)
{
    if (fd > g_max_event_num){
        return -1;
    }
    socket_event_s* socket_event_ptr = &ev_loop_s_ptr->socket_event_ptr[fd];
    socket_event_ptr->mask = 0;
    socket_event_ptr->fd = 0;
    socket_event_ptr->event_type =0;
    ev_io_stop(ev_loop_s_ptr->ev_loop_ptr,&socket_event_ptr->wather);

    if (fd == g_maxfd)
    {
        while(ev_loop_s_ptr->socket_event_ptr[g_maxfd].mask == 0 && g_maxfd > 0)
        {
            g_maxfd--;
        }
    }
    socket_event_ptr->client_data = NULL;
    return 0;
}

