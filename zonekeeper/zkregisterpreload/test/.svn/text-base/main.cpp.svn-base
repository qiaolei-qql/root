#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <iostream>
#include "global_var.h"
#include "zk_register_server.h"


int main(){
   LOG_INIT_EX("xl_log4cplus.properties");
  int a=10; 
   printf("y_init %d\n",a);
   LOGGER_DEBUG(g_logger,"debug g_logger");
   ZKRegisterServer::get_instance()->init();
   std::cout<<"c++"<<std::endl;
   sleep(10000);
    return 0;
 }

