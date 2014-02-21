#include "listen_thread.h" 
#include "net.h"
#include "base_header.h"
#include "SocketUtility.h"


static listen_thread_s g_listen_thread_s;

void* listen_thread_proc(void* arg)
{
    printf("listen_thread_proc run\n");
    assert(g_listen_thread_s.listen_ev_loop.ev_loop_ptr!= NULL);
    ev_run(g_listen_thread_s.listen_ev_loop.ev_loop_ptr,0);
}

void listen_thread_init()
{
    create_ev_loop(&g_listen_thread_s.listen_ev_loop);
    pthread_create(&g_listen_thread_s.thread_id, NULL, listen_thread_proc, NULL);
    conn_list_init();
    sched_yield();
}

static void accept_tcp_handler(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    printf("accept_tcp_handler \n");
    printf("accept_tcp_handler \n");
    assert(events == EV_READ);
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;
    int fd = socket_event_ptr->fd;   
    struct sockaddr_in addr;
    int client_fd;
    if (0 > (client_fd = accept_client(fd, &addr))){
        sprintf(stderr,"accept_client error %d \n", client_fd);
        return;
    }
    char ip[16] ={'\0'};
    uint32_t port;
    addr_to_ipport(&addr, ip, &port);

    set_nonblock(client_fd);
    // create conn
    char buf[600]={'0'};
    read(client_fd,buf,600);
    write(client_fd,buf,600);
    fflush(stdout);
    printf("accept_client %s %d \n\n\n",ip,port);
    printf("accept_client %s %d \n\n\n",ip,port);
    list_item_ptr(conn_list) conn_ptr = create_client(CONN_CLIENT, CONN_ESTABLISHED, client_fd);
    if (NULL == conn_ptr) {
        perror("create_client error");
        close(fd);
        return;
    }
    socket_event_ptr->client_data = (void*)conn_ptr;
    // dispath conn new
    dispath_new_conn(socket_event_ptr);

}
int add_listen(char* ip, int port, enum socket_event_type_e socket_type)
{
    struct sockaddr_in addr;
    int listen_fd = listen_to(ip,port,&addr,false);
    if (listen_fd < 0){
        printf( "listen_to err %s %d \n",ip,port);
        return -1;
    }
    printf( "listen_to %s %d\n",ip,port);
    g_listen_thread_s.listen_ev_loop.socket_event_ptr[listen_fd].event_type = socket_type;
    create_ev_event(&g_listen_thread_s.listen_ev_loop, listen_fd, EV_READ, accept_tcp_handler,NULL);
}

