#include "message_handler.h"
#include "socket_utility.h"
#include "thread_poll.h"
#include "connect_manager.h"



void add_new_conn_handler(void* arg1,void* arg2)
{
    assert(arg1 != NULL);
    conn_t* conn_ptr = (conn_t*)arg1;
    set_nonblock(conn_ptr->fd);
    log_debug("add_new_conn_handler conn %p, g_thread_id %d thread_id %d fd %d event_mask %d", conn_ptr,g_threadid,conn_ptr->thread_id,conn_ptr->fd, conn_ptr->event_mask);
    log_debug("socket_event_ptr %p",g_work_threads[g_threadid].work_ev.socket_event_ptr);
    create_ev_event(&g_work_threads[g_threadid].work_ev, conn_ptr->fd,EV_READ,client_conn_event_cb,(void*)conn_ptr);
}

void conn_write_data_handler(void* arg1,void* arg2)
{
    assert(arg1 != NULL);
    assert(arg2 != NULL);
    conn_t* conn_ptr = (conn_t*)arg1;
    io_buf_t* io_buf_ptr = (io_buf_t*)arg2;
    char buf[255];
    dump_conn(conn_ptr,buf);
    log_debug("conn_write_data_handler conn_ptr %p %s",conn_ptr,buf);
    int ret = send_data(conn_ptr->fd, io_buf_ptr->buf, io_buf_ptr->len);
    if (ret < 0){
        // error close conn
        char buf[255] ={'\0'};
        dump_conn(conn_ptr,buf);
        log_debug("send_data eror conn %s",buf);
        free_client(conn_ptr);
        return;
            //socket_event_ptr->client_data = NULL;
    }
    if (ret == io_buf_ptr->len){
        log_debug("send_data finish %d",ret);
        free(io_buf_ptr->buf);
        io_buf_ptr->buf = NULL;
        free(io_buf_ptr);
        io_buf_ptr = NULL;
        return;
    }
    log_debug("send_data un finish %d",ret);
    io_buf_ptr->offset += ret;
    list_item_new(async_buf_queue,buf_tmp);
    buf_tmp->io_buf =  *io_buf_ptr;
    free(io_buf_ptr);
    io_buf_ptr = NULL;
    list_push(conn_ptr->wbuffers, buf_tmp);
    // 防止从副注册事件
    create_ev_event(&g_work_threads[g_threadid].work_ev, conn_ptr->fd,EV_WRITE,client_conn_event_cb,(void*)conn_ptr);
}
