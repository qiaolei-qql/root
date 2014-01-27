#ifndef __SD_REDIS_SERVER_POOL_H__
#define __SD_REDIS_SERVER_POOL_H__

#include "SDRedisServer.h"
#include <boost/shared_ptr.hpp>
#include <common/SDPthreadLock.h>
#include <common/SDException.h>
#include <common/SDLogger.h>

#include <string>
#include <vector>


class SDRedisServerPool
{
public:
	SDRedisServerPool(unsigned poolsize, const std::string &id, const std::string& host, uint16_t port, uint16_t db_index);
	SDRedisServerPool(const SDRedisServerPool& rs_pool);

	virtual ~SDRedisServerPool();

	//init this redis server pool
	bool	Init();

	//get redis server from pool
	SDRedisServer_ptr get(bool autocreate = true);

	//return back redis server to pool
	void restore(SDRedisServer_ptr redis_server);

	inline std::string get_id() const {return m_id;}
	inline std::string	get_host() const {return m_host;}
	inline uint16_t	get_port() const {return m_port;}
	inline uint16_t get_db_index() const {return m_db_index;}
		
private:
	// in-- internal, which means caller should get the lock in advance!
	// don't call this method, after finish init
	void set_size_in(unsigned int size);	

	
	std::string m_id;
	std::string m_host;
	uint16_t	m_port;

	uint16_t m_db_index;
	
	std::vector<SDRedisServer_ptr> m_rspool_list;
	SDLock m_lock;

 private:
 	DECL_LOGGER(logger);
	
};


typedef boost::shared_ptr<SDRedisServerPool> SDRedisServerPool_ptr;


class SDAutoRedis
{
public:	
	SDAutoRedis(SDRedisServerPool_ptr pool, bool autocreate)
	:m_rs_pool(pool), m_autocreate(autocreate)
	{	
	
	}
		
	~SDAutoRedis()
	{
		if(m_redis_server)
		{
			m_rs_pool->restore(m_redis_server);
		}
	}

	
	SDAutoRedis(const SDAutoRedis& other)
	{
		m_rs_pool = other.m_rs_pool;
		m_autocreate = other.m_autocreate;
		m_redis_server.reset();
	}

	
	SDAutoRedis& operator=(const SDAutoRedis& other)
	{
		if (this != &other) {
			m_rs_pool = other.m_rs_pool;
			m_autocreate = other.m_autocreate;
			m_redis_server.reset();
		}

		return (*this);
	}

	
	SDRedisServer_ptr ptr()
	{
		if(!m_redis_server)
		{
			if(m_rs_pool)
			{
				m_redis_server = m_rs_pool->get(m_autocreate);
			}
		}
		
		return m_redis_server;
	}
	
private:
	SDRedisServer_ptr m_redis_server;
	SDRedisServerPool_ptr m_rs_pool;
	bool m_autocreate;
};

#endif // __SD_REDIS_SERVER_POOL_H__

