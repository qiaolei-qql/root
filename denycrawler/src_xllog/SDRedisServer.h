#ifndef __SD_REDIS_SERVER_H__
#define __SD_REDIS_SERVER_H__

#include <hiredis/hiredis.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <common/SDLogger.h>
//#include "SDCondValue.h"

/*
redisContext should not been shared, so SDRedisServer is noncopyable!
*/
class SDRedisServer: boost::noncopyable
{
public:
	SDRedisServer(std::string id,
						std::string host,
						uint16_t port,
						uint16_t db_index,	/*which db to use*/
						redisContext *redis_context = NULL);

	SDRedisServer(const SDRedisServer& other);

	~SDRedisServer();
	
	bool operator<(const SDRedisServer& other) const;

	// establish connection to (host,port,db)
	bool connect(uint32_t toms = 1500);

	// ping redis server
	bool ping();

	bool slaveofnoone();
	bool slaveof(const std::string host, uint16_t port);
	bool info(std::map<std::string,std::string>& info_map);
	bool sync_status();

	// select redis db
	bool select_db(uint16_t db_index);
		
	// close connection with redis server
	bool close();

	// hash get and if exist then update key timeout
	int hget2(const std::string &key, std::vector<Value> &values, uint32_t duration_time,bool is_set_expire);

	// hash get all filed if exist
	int hgetall(const std::string &key, std::vector<Value> &values, uint32_t duration_time,bool is_set_expire);

	//  hash set and then set key timeout
	int hset2(const std::string &key, std::vector<Value> &values, uint32_t duration_time);

	int hrem(const std::string &key, std::vector<Value> &values);
		
	// set add some members into the set for a key
	int sadd2(const std::string &key, std::vector<std::string> &members, uint32_t duration_time);

	// get all members in the set for a key
	int sget2(const std::string &key, std::vector<std::string> &members, uint32_t duration_time);

	// remove set members
	int srem(const std::string &key, std::vector<std::string> &members);

	// string get
	int strget2(const std::string &key, std::string &value, uint32_t duration_time);

	// string set
	int strset2(const std::string &key, std::string &value, uint32_t duration_time);
	
	// key exist， 
	int exist(const std::string &key, bool &exist);
	
	// remove key
	int remove(const std::string &key);

	// Set a timeout(s) on key
	int expire(const std::string &key, uint32_t duration_time);

	// incrby 

	int hincrby(const std::string &key,const std::string &filed,int32_t &increment);

	// 将本redis server上的key 迁移到dst redis server上 ，注意要先在dst redis server 上del 该key 否则迁移失败
	int migrate(std::string &dst_redis_ip, uint16_t dst_redis_port, const std::string &key, uint32_t db, uint32_t to_ms);

	// 和redis_server连接是否建立(但不保证连接一定可用)
	bool is_connected(){return (m_redis_context != NULL);}

	std::string dump() const
	{
		std::ostringstream oss;
		
		oss << "id:"       << m_id
			<< " host:"    << m_host
			<< " port:"    << m_port
			<< " db:"	      << m_db_index
			<< " this:"	   << this;
			
		return oss.str();
	}

private:
	int hget(const std::string &key, std::vector<Value> &values);

	int hgetall(const std::string &key, std::vector<Value> &values);

	int hset(const std::string &key, std::vector<Value> &values);

	int sadd(const std::string &key, std::vector<std::string> &members);

	int sget(const std::string &key, std::vector<std::string> &members);

	int strget(const std::string &key, std::string &value);

public:
	std::string m_id;		// redis_id
	std::string m_host;	// redis server host			
	uint16_t m_port;		// redis server port
	
	uint16_t m_db_index;
	bool m_in_use;		//if be used right now
	
	redisContext *m_redis_context;	// redis handle
	
private:
    LOG_CLS_DEC();
 
};


#define CHECK_REDIS_CONN(operation, ret_fail_value)\
    do {\
		if (!is_connected()) {\
			LOG4CPLUS_WARN(logger,(operation) << " fail, cos conneciton to " << dump() << " not established");\
			return ret_fail_value;\
		}\
	} while (0)

	
typedef boost::shared_ptr<SDRedisServer> SDRedisServer_ptr;

#endif	// __SD_REDIS_SERVER_H__

