#include <stdio.h>
#include "net.h"
#include "listen_thread.h"
#include "thread_poll.h"
#include "base_header.h"
void init_server()
{
    listen_thread_init();
    add_listen("0.0.0.0", 2000, LISTEN_CLIENT);
    work_thread_init(2);
}
int main(int argc, const char *argv[])
{
       
    init_server();
    while(true){

        sleep(1);
    }
    return 0;
}
