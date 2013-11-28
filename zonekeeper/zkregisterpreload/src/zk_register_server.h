/*
 * filename      : zk_register_server.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2013-09-13 15:59
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _ZK_REGISTER_SERVER_H_
#define _ZK_REGISTER_SERVER_H_
#include <string>
#include "global_var.h"

class ZKRegisterHandler;
class ZKRegisterServer
{
    public:
        static ZKRegisterServer* get_instance()
        {	
		if (_instance){
                	return _instance;
		}
		_instance = new ZKRegisterServer();
		return _instance;
        }
        static std::string name_mapper(const std::string& env_name);
    public:
        ~ZKRegisterServer();
        int init();
        int read_conf();

    private:
        static ZKRegisterServer *_instance;
        ZKRegisterServer();
        ZKRegisterServer(const ZKRegisterServer& zkrs);
        ZKRegisterServer* operator=(const ZKRegisterServer &zkrs);
        ZKRegisterHandler* m_zk_handler;
        std::string m_zk_register_root_path;
        std::string m_zk_register_node_name;
        std::string m_zk_register_node_path;
        std::string m_zk_host_list;
        int m_zk_timeout;
    LOG_CLS_DEC();   
};
#endif
