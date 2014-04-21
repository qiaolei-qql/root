#include "connect_manager.h"
#include "thread_poll.h"
#include "message_handler.h"

static conn_manager_t g_conn_manager ={0,UINT16_MAX,0,0};
static pthread_mutex_t g_conn_manager_lock;
void conn_manager_init()
{
    pthread_mutex_init(&g_conn_manager_lock,NULL);
}

int add_conn(conn_t* conn_ptr)
{
    conn_hash_t* conn_hash_ptr = NULL;
    pthread_mutex_lock(&g_conn_manager_lock);
    do {
        g_conn_manager.seq++;
        HASH_FIND_INT(g_conn_manager.conn_hash_head,&g_conn_manager.seq,conn_hash_ptr);
    }
    while(conn_hash_ptr);
    if (NULL == (conn_hash_ptr = (conn_hash_t*)malloc(sizeof(conn_hash_t))))
    {
        perror("malloc conn_hash_t failed");
        pthread_mutex_unlock(&g_conn_manager_lock);
        return -1;
    }
    conn_hash_ptr->conn_ptr = conn_ptr;
    conn_hash_ptr->handle = g_conn_manager.seq;
    conn_ptr->handle = g_conn_manager.seq;
    HASH_ADD_INT(g_conn_manager.conn_hash_head, handle, conn_hash_ptr);
    g_conn_manager.conn_num++;
    pthread_mutex_unlock(&g_conn_manager_lock);
    char buf[255];
    dump_conn(conn_ptr,buf);
    log_debug("add_conn %s",buf);
    return g_conn_manager.seq;
}
int delete_conn(int handle)
{
    conn_hash_t* conn_hash_ptr = NULL;
    pthread_mutex_lock(&g_conn_manager_lock);
    HASH_FIND_INT(g_conn_manager.conn_hash_head, &handle, conn_hash_ptr);
    if (NULL == conn_hash_ptr) {
        log_error("delete_conn handle %d NULL",handle);
        pthread_mutex_unlock(&g_conn_manager_lock);
        return -1;
    }
    char buf[255];
    dump_conn(conn_hash_ptr->conn_ptr,buf);
    log_debug("del_conn conn %s",buf);
    if (conn_hash_ptr->conn_ptr){
        if (conn_hash_ptr->conn_ptr->wbuffers) {
            list_item_ptr(async_buf_queue) buf_ptr = NULL;
            list_foreach(conn_hash_ptr->conn_ptr->wbuffers,buf_ptr)
            {
                if (!buf_ptr) break;
                if(buf_ptr->io_buf.buf) {
                    free(buf_ptr->io_buf.buf);
                    buf_ptr->io_buf.buf = NULL;
                }
                log_debug("free send queue %p fd %d",buf_ptr,conn_hash_ptr->conn_ptr->fd);
            }
        }
        close(conn_hash_ptr->conn_ptr->fd); 
        free(conn_hash_ptr->conn_ptr);
        conn_hash_ptr->conn_ptr = NULL;
    }
    log_debug("hash_head %p ,hash_ptr %p",g_conn_manager.conn_hash_head, conn_hash_ptr);
    HASH_DEL(g_conn_manager.conn_hash_head,conn_hash_ptr);
    g_conn_manager.conn_num--;
    pthread_mutex_unlock(&g_conn_manager_lock);
    return 0;
}

conn_t* get_conn(int handle)
{
    conn_hash_t* conn_hash_ptr = NULL;
    pthread_mutex_lock(&g_conn_manager_lock);
    HASH_FIND_INT(g_conn_manager.conn_hash_head, &handle, conn_hash_ptr);
    pthread_mutex_unlock(&g_conn_manager_lock);
    if(conn_hash_ptr)
    {
        char buf[255] ={'\0'};
        dump_conn(conn_hash_ptr->conn_ptr, buf);
        log_debug("get conn %s",buf);
        return conn_hash_ptr->conn_ptr;
    }
    return NULL;
}

// thread safe ??
int conn_send_data(int handle, char* snd_buf, int bytes)
{
    conn_t* conn_ptr = get_conn(handle);
    if (NULL == conn_ptr) {
        log_error("get_conn NULL handle %d",handle);
        return -1;
    }
    io_buf_t* io_buf_ptr = (io_buf_t*)malloc(sizeof(io_buf_t));
    if(!io_buf_ptr) return 0;
    io_buf_ptr->buf = snd_buf;
    io_buf_ptr->len = bytes;
    io_buf_ptr->offset = 0;
    char buf[255];
    dump_conn(conn_ptr,buf);
    log_debug("conn_send_data conn_ptr %p %s",conn_ptr,buf);
    post_message(conn_ptr->thread_id,conn_write_data_handler,(void*)conn_ptr,(void*)io_buf_ptr);
    return 0;
}

conn_t* create_client(enum conn_type_e conn_type, enum conn_stat_e conn_stat, int fd)
{
    if (g_conn_manager.conn_num > g_conn_manager.conn_max_num) {
        // log
        return NULL;
    }
    conn_t* conn_ptr = (conn_t*)malloc(sizeof(conn_t));
    if (!conn_ptr) {
        perror("conn_hash_t malloc failed");
        return NULL;
    }
    conn_ptr->conn_type = conn_type;
    conn_ptr->conn_stat = conn_stat;
    conn_ptr->fd = fd;
    conn_ptr->event_mask = EV_READ;
    
    conn_ptr->rbuf = (char*)malloc(1024);
    assert(conn_ptr->rbuf != NULL);
    conn_ptr->rbytes = 0;
    conn_ptr->rsize = 1024;

/*    conn_ptr->wbuf = NULL;
    conn_ptr->wbytes = 0;
    conn_ptr->wsize = 1024;
    */

    list_new(async_buf_queue,buf_head);
    assert(buf_head != NULL);
    conn_ptr->wbuffers =  buf_head;

    conn_ptr->thread_id= -1;
    
    add_conn(conn_ptr);
    return conn_ptr;
}

int free_client(conn_t* conn_ptr)
{
    assert(conn_ptr != NULL);
    //free conn
    delete_ev_event(&g_work_threads[g_threadid].work_ev,conn_ptr->fd);
    if(conn_ptr->rbuf) {
        free(conn_ptr->rbuf);
        conn_ptr->rbuf = NULL;
    }
    delete_conn(conn_ptr->handle);
    return 0;
}
void dump_conn(conn_t* conn_ptr,char* buf)
{
    assert(conn_ptr != NULL);
    sprintf(buf,"conn fd %d handle %d threadid %d rbytes %d rsize %d wbuffers %p",conn_ptr->fd,conn_ptr->handle,conn_ptr->thread_id,conn_ptr->rbytes,conn_ptr->rsize,conn_ptr->wbuffers);

}
