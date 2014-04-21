/*
 * filename      : connect_manager.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2014-02-28 15:04
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _CONNECT_MANAGER_H_
#define _CONNECT_MANAGER_H_
#include "uthash.h"
#include "net.h"

/* conn manager*/
typedef struct{
    conn_t* conn_ptr;
    uint32_t handle;
    UT_hash_handle hh;
}conn_hash_t;

typedef struct {
    int conn_num;
    int conn_max_num;
    uint32_t seq;
    conn_hash_t* conn_hash_head;
}conn_manager_t;

int add_conn(conn_t* conn_ptr);
int delete_conn(int handle);
conn_t* get_conn(int handle);
int free_client(conn_t* conn_ptr);
void conn_manager_init();

int conn_send_data(int handle, char* snd_buf, int bytes);

conn_t* create_client(enum conn_type_e conn_type, enum conn_stat_e conn_stat, int fd);

void dump_conn(conn_t* conn_ptr,char* buf);
#endif
