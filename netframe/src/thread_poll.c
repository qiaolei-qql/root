#include "thread_poll.h"
#include "client_processor.h"
#include "socket_utility.h"
#include "connect_manager.h"

LIBEV_WORK_THREAD* g_work_threads = NULL;
static int g_work_thread_num = 0;
static int g_last_thread_id = -1;
__thread  volatile uint64_t g_threadid;



static void pipe_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    assert(events == EV_READ);   
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;
    int notify_read_fd = socket_event_ptr->fd;
    char buf[1] = {0};
    while(1 == read(notify_read_fd, buf, 1)) 
    {}
    if (errno != EAGAIN && errno != EINTR) {
        fprintf(stderr, "cant not read from pipe\n");
        return ;
    }
    list_item_ptr(msg_queue) msg_ptr;
    list_shift(g_work_threads[g_threadid].thread_msgs, msg_ptr);
    if(NULL == msg_ptr) {
        fprintf(stderr, "Shift work thread conn_list NULL ");
        return;
    }
    log_debug("read PIPE msg_ptr : %p msg_handler: %p arg1 %p ",msg_ptr, msg_ptr->msg.msg_handler ,msg_ptr->msg.arg1);
    // call back
    (*msg_ptr->msg.msg_handler)(msg_ptr->msg.arg1,msg_ptr->msg.arg2);
}

static void* work_thread_proc(void* arg)
{
    g_threadid = (intptr_t)arg;
    log_debug("thread_id: %ld \n \n",g_threadid);
    int fd = g_work_threads[g_threadid].notify_read_fd;
    g_work_threads[g_threadid].work_ev.socket_event_ptr[fd].event_type = PIPE_EVENT;   
    create_ev_event(&g_work_threads[g_threadid].work_ev, g_work_threads[g_threadid].notify_read_fd, EV_READ, pipe_event_cb,NULL);

    log_debug( "work_thread_proc run\n\n");
    ev_run(g_work_threads[g_threadid].work_ev.ev_loop_ptr,0);
    return NULL;
}

int post_message(int thread_id, message_handler* handler, void* arg1, void* arg2)
{
    list_item_new(msg_queue,msg_tmp);
    msg_tmp->msg.msg_handler = handler;
    msg_tmp->msg.arg1 = arg1;
    msg_tmp->msg.arg2 = arg2;

    log_debug("write pipe thread_id %d msg_ptr : %p msg_handler: %p arg1 %p ",thread_id, msg_tmp, msg_tmp->msg.msg_handler ,msg_tmp->msg.arg1);

    list_push(g_work_threads[thread_id].thread_msgs, msg_tmp);
    char buf[1] = {'1'};
    if(1 != write(g_work_threads[thread_id].notify_write_fd,buf,1)) {
        perror("write to workthread pipe error");
        return -1;
    }
    return 0;
}

int dispath_new_conn(conn_t* conn_ptr)
{
    int thread_id = (g_last_thread_id + 1)%g_work_thread_num;
    g_last_thread_id = thread_id;
    conn_ptr->thread_id = thread_id;
    return post_message(thread_id, add_new_conn_handler, (void*)conn_ptr, NULL);
}

void client_conn_event_cb(struct ev_loop* loop, struct ev_io* io_w, int events)
{
    assert(io_w != NULL);
    socket_event_s* socket_event_ptr = (socket_event_s*)io_w;

    log_debug("socket_event_ptr %p", socket_event_ptr);
    int fd = socket_event_ptr->fd;
    conn_t* conn_ptr = (conn_t*)socket_event_ptr->client_data;


    log_debug("conn_evevnt_cb conn_ptr->fd %d fd %d",conn_ptr->fd, fd);
    if (events & EV_READ)
    {
        log_debug("fd %d rsize %d, rbytes %d events %d ",conn_ptr->fd, conn_ptr->rsize, conn_ptr->rbytes,events);
        int cnt = recv_data(conn_ptr->fd,&conn_ptr->rbuf,&conn_ptr->rbytes, &conn_ptr->rsize);
        log_debug("fd %d rsize %d, rbytes %d  ",conn_ptr->fd, conn_ptr->rsize, conn_ptr->rbytes);
        if (cnt<0) {
            log_debug("recv error cnt %d,fd %d ",cnt,fd);
            //free conn
            free_client(conn_ptr);
            return ;
        }
        client_processor(conn_ptr);
    }
    if (events & EV_WRITE)
    {
        while(1){
            log_debug("wbuffers size %d, events %d ",list_size(conn_ptr->wbuffers), events);
            list_item_ptr(async_buf_queue) buf_ptr;
            list_shift(conn_ptr->wbuffers, buf_ptr);
            if (NULL == buf_ptr) {
                log_error("list shift async_buf_queue NULL");
                return;
            }
            io_buf_t io_buf = buf_ptr->io_buf;
            int ret = send_data(fd, io_buf.buf+io_buf.offset, io_buf.len);
            if (ret < 0) {
                // close conn
                log_error("conn send_data error");
                free_client(conn_ptr);
                return ;
            }
            if (ret == io_buf.len -io_buf.offset) {
                free(io_buf.buf);
                io_buf.buf = NULL;
                free(buf_ptr);
                buf_ptr = NULL;
                delete_ev_event(&g_work_threads[g_threadid].work_ev,fd);
                create_ev_event(&g_work_threads[g_threadid].work_ev,fd,EV_READ,client_conn_event_cb,(void*)conn_ptr);
                break;
            }
            else{
                buf_ptr->io_buf.offset += ret;
                list_unshift(conn_ptr->wbuffers, buf_ptr);
            }
            log_debug("fd %d buf_ptr %p buf_len %d offset %d",conn_ptr->fd, buf_ptr, buf_ptr->io_buf.len, buf_ptr->io_buf.offset);
        }
    }
}


int is_work_thread()
{
    return pthread_self() == g_work_threads[g_threadid].thread_id;
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
        list_new(msg_queue,msg_tmp);
        assert(msg_tmp != NULL);
        g_work_threads[i].thread_msgs = msg_tmp;

        set_nonblock(g_work_threads[i].notify_read_fd);
        create_ev_loop(&g_work_threads[i].work_ev);
        log_debug("evloop[%d]: %p %p",i,g_work_threads[i].work_ev.ev_loop_ptr,g_work_threads[i].work_ev.socket_event_ptr);
        pthread_create(&g_work_threads[i].thread_id, NULL, work_thread_proc,(void*)(intptr_t)i);
        sched_yield();
    }
}
