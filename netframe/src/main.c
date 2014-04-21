#include <stdio.h>
#include "net.h"
#include "listen_thread.h"
#include "thread_poll.h"
#include "connect_manager.h"

void init_server()
{
    log_open("./log/debug.log");
    listen_thread_init();
    add_listen("127.0.0.1", 2000, LISTEN_CLIENT);
    listen_thread_run();
    conn_manager_init();
    work_thread_init(2);
}
int main(int argc, const char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);
       
    init_server();
    while(true){

        sleep(10);
    }
    return 0;
}
