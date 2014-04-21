#include "listen_thread.h" 
#include "net.h"
#include "socket_utility.h"
#include "connect_manager.h"
#include "thread_poll.h"


static listen_thread_s g_listen_thread_s;

static void* listen_thread_proc(void* arg)
{
    log_debug("listen_thread_proc run\n");
    assert(g_listen_thread_s.listen_ev_loop.ev_loop_ptr!= NULL);
    ev_run(g_listen_thread_s.listen_ev_loop.ev_loop_ptr,0);
    return NULL;
}

void listen_thread_init()
{
/*    if (!g_listen_thread_s.listen_list_head){
        list_new(listen_list, listen_list_head_ptr);
        assert(listen_list_head_ptr!= NULL);
        g_listen_thread_s.listen_list_head = listen_list_head_ptr;
    }
    */
    create_ev_loop(&g_listen_thread_s.listen_ev_loop);
}
void listen_thread_run(){
    if (0 != pthread_create(&g_listen_thread_s.thread_id, NULL, listen_thread_proc, NULL)){
        perror("pthread_create failed");
        exit(-1);
    }
    sched_yield();
}

static void accept_tcp_handler(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    log_debug("accept_tcp_handler ");
    fflush(NULL);
    assert(events == EV_READ);
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;
    int fd = socket_event_ptr->fd;
    struct sockaddr_in addr;
    int accept_fd;
    if (0 > (accept_fd = accept_client(fd, &addr))){
        sprintf(stderr,"accept_client error %d \n", accept_fd);
        return;
    }
    char ip[16] ={'\0'};
    uint32_t port;
    addr_to_ipport(&addr, ip, &port);
    log_debug("accept_client %s %d fd %d \n\n\n",ip,port, accept_fd);

    conn_t* conn_ptr = create_client(CONN_CLIENT, CONN_ESTABLISHED, accept_fd);
    if (NULL == conn_ptr) {
        perror("create_client error");
        close(fd);
        return;
    }
    // dispath conn new
    dispath_new_conn(conn_ptr);

}
int add_listen(char* ip, int port, enum socket_event_type_e socket_type)
{
    struct sockaddr_in addr;
    int listen_fd = listen_to(ip,port,&addr,false);
    if (listen_fd < 0){
        log_debug( "listen_to err %s %d \n",ip,port);
        return -1;
    }
    log_debug( "listen_to %s %d\n",ip,port);
    g_listen_thread_s.listen_ev_loop.socket_event_ptr[listen_fd].event_type = socket_type;
    create_ev_event(&g_listen_thread_s.listen_ev_loop, listen_fd, EV_READ, accept_tcp_handler,NULL);
    return 0;
}


