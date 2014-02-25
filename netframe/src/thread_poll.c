#include "thread_poll.h"


static LIBEV_WORK_THREAD* g_work_threads;
static g_work_thread_num = 0;
__thread  volatile uint64_t g_thread_id; 
static g_last_thread_id = -1;

static void client_conn_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;
    int fd = socket_event_ptr->fd;
    list_item_ptr(conn_list) conn_ptr = (list_item_ptr(conn_list))socket_event_ptr->client_data;

    if (events & EV_READ)
    {
        log_debug("rsize %d, rbytes %d events %d ",conn_ptr->rsize, conn_ptr->rbytes,events);

        int cnt = recv_data(fd,&conn_ptr->rbuf,&conn_ptr->rbytes, &conn_ptr->rsize);
        log_debug("rsize %d, rbytes %d  ",conn_ptr->rsize, conn_ptr->rbytes);
        if (cnt<0) {
            log_debug("recv error cnt %d,fd %d",cnt,fd);
            //free conn
            delete_ev_event(&g_work_threads[g_thread_id].work_ev,fd);
            close(fd);
            free(conn_ptr);
            socket_event_ptr->client_data = NULL;
            return ;
        }
    }
    if (events & EV_WRITE)
    {
        log_debug("write event fd %d",fd);
    }
}

static void* work_thread_proc(void* arg)
{
    g_thread_id = (intptr_t)arg;
    log_debug("thread_id: %d \n \n",g_thread_id);
    int fd = g_work_threads[g_thread_id].notify_read_fd;
    g_work_threads[g_thread_id].work_ev.socket_event_ptr[fd].event_type = PIPE_EVENT;   
    create_ev_event(&g_work_threads[g_thread_id].work_ev, g_work_threads[g_thread_id].notify_read_fd, EV_READ, pipe_event_cb,NULL);

    log_debug( "work_thread_proc run\n\n");
    ev_run(g_work_threads[g_thread_id].work_ev.ev_loop_ptr,0);
}

static void pipe_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    assert(events == EV_READ);   
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;
    int notify_read_fd = socket_event_ptr->fd;
    char buf[1] = {0};
    if (1 != read(notify_read_fd, buf, 1)) {
        fprintf(stderr, "cant not read from pipe\n");
    }
    list_item_ptr(conn_list) conn_ptr;
    list_shift(g_work_threads[g_thread_id].new_conns, conn_ptr);
    if(NULL == conn_ptr) {
        fprintf(stderr, "Shift work thread conn_list NULL ");
        return;
    }
    set_nonblock(conn_ptr->fd);
    log_debug("read pipe suc %p, thread_id %d fd %d event_mask %d", conn_ptr,g_thread_id,conn_ptr->fd, conn_ptr->event_mask );
    create_ev_event(&g_work_threads[g_thread_id].work_ev, conn_ptr->fd,EV_READ/*conn_ptr->event_mask*/,client_conn_event_cb,conn_ptr);

}

int is_work_thread()
{
    return pthread_self() == g_work_threads[g_thread_id].thread_id;
}
void work_thread_init(int nwork_threads)
{
    g_work_threads = (LIBEV_WORK_THREAD*)malloc(sizeof(LIBEV_WORK_THREAD)* nwork_threads);
    assert(g_work_threads != NULL);
    assert(nwork_threads > 0);
    g_work_thread_num = nwork_threads;

    for (int i = 0; i < nwork_threads; i++) {
        /* code */
        int fds[2];
        if (pipe(fds)){
            perror("cant create notify pipe");
            exit(-1);
        }
        g_work_threads[i].notify_read_fd = fds[0];
        g_work_threads[i].notify_write_fd = fds[1];
        
        log_debug("pipe:%d %d ",fds[0],fds[1]);
        list_new(conn_list,conns_tmp);
        assert(conns_tmp != NULL);
        g_work_threads[i].new_conns  = conns_tmp;

        set_nonblock(g_work_threads[i].notify_read_fd);
        create_ev_loop(&g_work_threads[i].work_ev);
        pthread_create(&g_work_threads[i].thread_id, NULL, work_thread_proc,(void*)(intptr_t)i);       
        sched_yield();
    }
}

int dispath_new_conn(socket_event_s* socket_event_ptr)
{
    int thread_id = (g_last_thread_id + 1)%g_work_thread_num;
    g_last_thread_id = thread_id;
    list_item_ptr(conn_list) conn_ptr = (list_item_ptr(conn_list))socket_event_ptr->client_data;
    list_push(g_work_threads[thread_id].new_conns,conn_ptr);
    log_debug("write pipe suc push %p thread_id %d fd %d  ", conn_ptr, thread_id, conn_ptr->fd);
    char buf[1] = {'1'};
    if(1 != write(g_work_threads[thread_id].notify_write_fd,buf,1)) {
        perror("write to workthread pipe error");
    }
}

