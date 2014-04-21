/*
 * filename      : message_handler.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2014-02-28 16:33
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _MESSAGE_HANDLER_H_
#define _MESSAGE_HANDLER_H_
#include "queue.h"
/* thread msg queue*/


typedef void message_handler(void* arg1,void* arg2);

typedef struct {
    message_handler* msg_handler;
    void* arg1;
    void* arg2;
}thread_msg_t;

list_def(msg_queue);

struct list_item_name(msg_queue)
{
    thread_msg_t msg;
    list_next_ptr(msg_queue);
};

void add_new_conn_handler(void* arg1,void* arg2);
void conn_write_data_handler(void* arg1,void* arg2);
#endif
