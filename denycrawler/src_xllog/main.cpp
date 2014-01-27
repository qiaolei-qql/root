#include "base_header.h"


int main(){
   LOG_INIT_EX("xl_log4cplus.properties");
  int a=10; 
   LOGGER_DEBUG(g_logger,"debug g_logger");
   std::cout<<"c++"<<std::endl;
   sleep(10000);
    return 0;
}

