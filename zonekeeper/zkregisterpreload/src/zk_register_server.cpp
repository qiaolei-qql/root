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

#include "zk_register_server.h"
#include "zk_register_handler.h"
#include <boost/program_options.hpp>
#include <boost/assign.hpp>

//ZKRegisterServer* ZKRegisterServer::_instance =  new ZKRegisterServer();
ZKRegisterServer* ZKRegisterServer::_instance = NULL; 

ZKRegisterServer::ZKRegisterServer()
{
	LOG_DEBUG( __FUNCTION__ << "(" << this << ")");
}
ZKRegisterServer::~ZKRegisterServer()
{
    LOG_DEBUG( __FUNCTION__ << "(" << this << ")");
    if (m_zk_handler){
        delete m_zk_handler;
        m_zk_handler = NULL;
    }
}

int ZKRegisterServer::init()
{
    read_conf();

    m_zk_handler = new ZKRegisterHandler(m_zk_host_list,m_zk_timeout);
    if (!m_zk_handler){
        LOG_ERROR("m_zk_handler init error");
        return -1;
    }
    int ret = m_zk_handler->init(m_zk_register_root_path,m_zk_register_node_name);
    if (ret <0){
        return -1;
    }
    return 0;
    //m_zk_handler->znode_exist(path,exist,true);
}
std::string ZKRegisterServer::name_mapper(const std::string& env_name)
{
    using namespace boost::assign;
    static map<std::string,std::string> nm = 
        map_list_of("ZK_REGISTER_PATH","ZK_REGISTER_PATH")
                ("ZK_REGISTER_ZKHOST_LIST","ZK_REGISTER_ZKHOST_LIST")
                ("ZK_REGISTER_ZKTIMEOUT","ZK_REGISTER_ZKTIMEOUT")
                ("ZK_REGISTER_NODE_NAME","ZK_REGISTER_NODE_NAME");
    return nm[env_name];
}
int ZKRegisterServer::read_conf()
{
    boost::program_options::options_description options("enviroment optione");
    options.add_options()
        ("ZK_REGISTER_PATH",boost::program_options::value<std::string>(&m_zk_register_root_path)->default_value("/redis_cluster"),"zk_register_path")
        ("ZK_REGISTER_ZKHOST_LIST",boost::program_options::value<std::string>(&m_zk_host_list)->default_value("0.0.0.0:2181"),"zk host list")
        ("ZK_REGISTER_ZKTIMEOUT",boost::program_options::value<int>(&m_zk_timeout)->default_value(3000),"zk time out")
        ("ZK_REGISTER_NODE_NAME",boost::program_options::value<std::string>(&m_zk_register_node_name)->default_value("127.0.0.1:0000"),"ZK_REGISTER_NODE_NAME");
    boost::program_options::variables_map vmap;
    boost::program_options::store(boost::program_options::parse_environment(options,name_mapper),vmap);
    boost::program_options::notify(vmap);
    m_zk_register_node_path = m_zk_register_root_path + "/" + m_zk_register_node_name;
    LOG_DEBUG("read ZK_REGISTER_PATH:" << m_zk_register_node_path);
	return 0;
}
