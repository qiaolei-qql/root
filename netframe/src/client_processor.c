#include "base_header.h"
#include "client_processor.h"
#include "connect_manager.h"

int client_processor(conn_t* conn_ptr)
{

    char *buf = (char*)malloc(3069);
    memset(buf,0,3069);
    int bytes = conn_ptr->rbytes;
    memcpy(buf,conn_ptr->rbuf,conn_ptr->rbytes);
    conn_ptr->rbytes =0;
    buf[conn_ptr->rbytes] = '\0';
    log_debug("client_processor conn fd:%d ", conn_ptr->fd);
    conn_send_data(conn_ptr->handle,buf,bytes);
    return 0;
}
