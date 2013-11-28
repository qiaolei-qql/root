#include "zk_register_handler.h"
ZKRegisterHandler::ZKRegisterHandler(const std::string& host_list, int recv_timeout)
:ZKSessionHandler(host_list,recv_timeout)
{
	LOG_DEBUG( __FUNCTION__ << "(" << this << ")");
}
ZKRegisterHandler::~ZKRegisterHandler()
{
	LOG_DEBUG( __FUNCTION__ << "(" << this << ")");
}
int ZKRegisterHandler::init(std::string& root_path,const std::string& node_name)
{
    m_node_path = root_path + "/" + node_name;
    LOG_DEBUG("ZKRegisterHandler::init");
    if( 0 != create_znode(root_path,"")){
        return -1;
    }
    if( 0 !=create_znode(m_node_path,"",ZOO_EPHEMERAL)){
        return -2;
    }
    bool exist= false;
    znode_exist(m_node_path, exist,true);
    LOG_DEBUG("node _path"<< m_node_path << "exit:"<<exist);
    return 0;
}
int ZKRegisterHandler::process_seesion_expired_recovering()
{
    LOG_DEBUG("process_seesion_expired_recovering ...");
    create_znode(m_node_path,"",ZOO_EPHEMERAL);
    return 0;
}
