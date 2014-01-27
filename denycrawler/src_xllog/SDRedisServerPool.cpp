#include "SDRedisServerPool.h"
#include <sys/time.h>
#include <errno.h>

using namespace std;

IMPL_LOGGER(SDRedisServerPool,logger);


SDRedisServerPool::SDRedisServerPool(unsigned poolsize, const std::string &id, const std::string& host, uint16_t port, uint16_t db_index)
:m_id(id),m_host(host),m_port(port),m_db_index(db_index)
{
	//SDAutoLock auto_lock(m_lock);
	
	set_size_in(poolsize);
}

// deep copy construct
SDRedisServerPool::SDRedisServerPool(const SDRedisServerPool& rs_pool)
{
	//SDAutoLock auto_lock(m_lock);
	
	m_id = rs_pool.get_id();
	m_host = rs_pool.get_host();
	m_port =  rs_pool.get_port();
	m_db_index = rs_pool.get_db_index();
		
	set_size_in(m_rspool_list.size());
}

SDRedisServerPool::~SDRedisServerPool()
{
	SDAutoLock auto_lock(m_lock);
	
	m_rspool_list.clear();
}

bool SDRedisServerPool::Init()
{
	SDAutoLock auto_lock(m_lock);
	
	bool can_connect = false;
	uint32_t succ_num = 0;
	for(unsigned i = 0; i < m_rspool_list.size(); i++)
	{	
		if(m_rspool_list[i]&& m_rspool_list[i]->connect())
		{
			succ_num++;
			can_connect = true;	//if at least one connection succ, we think redis server can be acessable
		}
	}

	LOG4CPLUS_DEBUG(logger, "connect succ rate: " << succ_num << "/" << m_rspool_list.size());
	
	return can_connect;
}

SDRedisServer_ptr SDRedisServerPool::get(bool autocreate)
{
	SDRedisServer_ptr redis_server;

	SDAutoLock auto_lock(m_lock);
	
	// not real round robin, conns in the front have high priority, connection at the tail may starve and timeout.
	for(unsigned i = 0; i < m_rspool_list.size(); i++)
	{
		if(m_rspool_list[i] && !m_rspool_list[i]->m_in_use)
		{
			m_rspool_list[i]->m_in_use = true;
			redis_server =  m_rspool_list[i];
			LOG4CPLUS_DEBUG(logger, "get a free connection["<< i << "]:" << redis_server->dump() << " from conn pool");
			
			break;
		}
	}
	
	// all connection are used, expand conn pool if needed
	if(!redis_server && autocreate)
	{
		redis_server = SDRedisServer_ptr(new SDRedisServer(m_id, m_host, m_port, m_db_index));
		LOG4CPLUS_WARN(logger, "no free connection, create a new one " << redis_server->dump() << " in conn pool");
		
		redis_server->connect();
		
		redis_server->m_in_use = true;
		m_rspool_list.push_back(redis_server);
	}

	return redis_server;
}

void SDRedisServerPool::restore(SDRedisServer_ptr redis_server)
{
	SDAutoLock auto_lock(m_lock);
	
	for(unsigned i = 0; i < m_rspool_list.size(); i++)
	{
		if(m_rspool_list[i] == redis_server)	
		{
			LOG4CPLUS_DEBUG(logger, "restore a connection[" << i << "]:" << redis_server->dump() << " to conn pool succ");
			m_rspool_list[i]->m_in_use = false;
			break;
		}
	}
}


void SDRedisServerPool::set_size_in(unsigned int size)
{	
	LOG4CPLUS_DEBUG(logger,"current pool size:" << m_rspool_list.size() << ", set to:" << size);
	
	for(unsigned i = 0; i < size; i++)
	{
		m_rspool_list.push_back(SDRedisServer_ptr(new SDRedisServer(m_id, m_host, m_port, m_db_index)));
	}
}


