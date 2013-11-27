#include "SDRedisServerPool.h"
#include <sys/time.h>
#include <errno.h>

using namespace std;

IMPL_LOGGER(SDRedisServerPool,logger);


SDRedisServerPool::SDRedisServerPool(const std::string& host, uint16_t port, uint16_t db_index)
:m_host(host),m_port(port),m_db_index(db_index),m_conn_num(0)
{
}

SDRedisServerPool::~SDRedisServerPool()
{
	m_conn_ptrs.clear();
}

SDRedisServer_ptr SDRedisServerPool::get()
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
		redis_server = SDRedisServer_ptr(new SDRedisServer("0",m_host, m_port, m_db_index),bind(&SDRedisServerPool::put, this, _1));
        return redis_server;
    }
    redis_server = m_conn_ptrs.front();
    m_conn_ptrs.pop_front();
    return redis_server;
}
/*
void SDRedisServerPool::put(SDRedisServer_ptr redis_server)
{
    boost::mutex::scoped_lock locked(m_mutex);	
    m_conn_ptrs.push_back(redis_server);
}
*/

void SDRedisServerPool::put(SDRedisServer* redis_server)
{
    boost::mutex::scoped_lock locked(m_mutex);	
	SDRedisServer_ptr redis_ptr;
	redis_ptr = SDRedisServer_ptr(redis_server,bind(&SDRedisServerPool::put, this, _1));
    m_conn_ptrs.push_back(redis_ptr);
    LOG4CPLUS_DEBUG(logger,"conn put ");
}
