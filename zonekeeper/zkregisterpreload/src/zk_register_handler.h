/*
 * filename      : ZKRegisterHandler.h
 * descriptor    :  
 * author        : fengyajie
 * create time   : 2013-09-17 15:45
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _ZKREGISTERHANDLER_H_
#define _ZKREGISTERHANDLER_H_
#include "global_var.h"
#include "zk_session_handler.h"
class ZKRegisterHandler : public ZKSessionHandler
{
    public:
       ZKRegisterHandler(const std::string& host_list, int recv_timeout); 
       ~ZKRegisterHandler(); 
       int init(std::string& root_path,const std::string& node_name);
       virtual int process_seesion_expired_recovering();
    private:       
       std::string m_node_path;
    LOG_CLS_DEC();
};
#endif
