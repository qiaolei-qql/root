#include "SDRedisServerPoolSingleton.h"
#include <sys/time.h>
#include <errno.h>

using namespace std;

IMPL_LOGGER(SDRedisServerPoolSingleton,logger);

SDRedisServerPoolSingleton* SDRedisServerPoolSingleton::m_instance = new SDRedisServerPoolSingleton();

SDRedisServerPoolSingleton::SDRedisServerPoolSingleton()
:m_host(""),m_port(0),m_db_index(0),m_conn_num(0)
{
}

SDRedisServerPoolSingleton::~SDRedisServerPoolSingleton()
{
	m_conn_ptrs.clear();
}

bool SDRedisServerPoolSingleton::init(const std::string& host, uint16_t port, uint16_t db_index)
{
    m_host = host;
    m_port = port;
    m_db_index = db_index;
    return true;
}

SDRedisServer_ptr SDRedisServerPoolSingleton::get()
{
	SDRedisServer_ptr redis_server;
    boost::mutex::scoped_lock locked(m_mutex);

    if (0 == m_conn_ptrs.size() )
    {
        m_conn_num++;
        if (m_conn_num > 40){
            LOG4CPLUS_WARN(logger,"redis pool conn num :" << m_conn_num);
        }
        LOG4CPLUS_WARN(logger,"redis pool conn num :" << m_conn_num);
		redis_server = SDRedisServer_ptr(new SDRedisServer("0",m_host, m_port, m_db_index),bind(&SDRedisServerPoolSingleton::put, this, _1));
        return redis_server;
    }
    redis_server = m_conn_ptrs.front();
    m_conn_ptrs.pop_front();
    return redis_server;
}
/*
void SDRedisServerPoolSingleton::put(SDRedisServer_ptr redis_server)
{
    boost::mutex::scoped_lock locked(m_mutex);	
    m_conn_ptrs.push_back(redis_server);
}
*/

void SDRedisServerPoolSingleton::put(SDRedisServer* redis_server)
{
	SDRedisServer_ptr redis_ptr;
	redis_ptr = SDRedisServer_ptr(redis_server,bind(&SDRedisServerPoolSingleton::put, this, _1));
    boost::mutex::scoped_lock locked(m_mutex);	
    m_conn_ptrs.push_back(redis_ptr);
    LOG4CPLUS_DEBUG(logger,"conn put ");
}
