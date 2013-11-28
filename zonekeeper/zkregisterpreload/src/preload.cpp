#include "global_var.h"
#include "zk_register_server.h"

void __attribute__ ((constructor)) my_init(void ){
	//sleep(10000);
	LOG_INIT_EX("xl_log4cplus.properties");
	LOGGER_IMP_EX(g_logger, "g_logger");
	sleep(3);
	LOGGER_DEBUG(g_logger,"debug ZKRegisterServer::_instance: "<< ZKRegisterServer::get_instance());
	ZKRegisterServer::get_instance()->init();
}

void __attribute__ ((destructor)) my_fini(void ){
	delete ZKRegisterServer::get_instance();
	LOGGER_DEBUG(g_logger,"debug ZKRegisterServer::_instance: "<< ZKRegisterServer::get_instance());
}
