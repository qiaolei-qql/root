#ifndef __SD_REDIS_STATUS_CHECKER_H__
#define __SD_REDIS_STATUS_CHECKER_H__

#include <common/SDLogger.h>
#include <common/SDThreadPool.h>
#include <common/SDPthreadLock.h>
#include <common/SDUtility.h>
#include "SDServerDispatcher.h"
#include "SDRedisServerManager.h"
#include "disconf/ZKDisConf.h"
#include <string>
#include <set>

class SDRedisDispatcher;

class SDRedisStatusChecker
{
public:
	static const uint32_t REDIS_UNKNOWN = 0;
	static const uint32_t REDIS_LIVE     = 1;
	static const uint32_t REDIS_DEAD    = 2;

	// transient status
	static const uint32_t REDIS_RELIVE  = 3;	
	static const uint32_t REDIS_DOWN   = 4;
	
public:
    	SDRedisStatusChecker(uint16_t info_type, std::string service_name);
	
    	~SDRedisStatusChecker();

    	bool init(SDServerDispatcher_ptr dispatcher, SDRedisServerManager_ptr rs_manager);

	bool add_redis_server(const std::string &redis_id);

	bool del_redis_server(const std::string &redis_id);
	
	std::string get_statistic(int reset);
	
	void routine();
	
private:	
	// notify dispatcher
	bool notify(const std::string &redis_id, uint32_t status, uint32_t last_status);

	// check redis server status
	uint32_t check_redis_status(const std::string &redis_id);

	// report zk redis status
	bool report(std::string &redis_id, uint32_t status, bool force);
		
	// called by others to sync redis server status to local map
	// this is a callback func to dis_conf module.
	bool sync_value(const std::string &key, std::string value);

	bool node_change(const std::string &key);

	// key_path --> redis_id
	bool keytoid(const std::string &key, std::string &redis_id);

	// redis_id --> key_path
	bool idtokey(const std::string &redis_id, std::string &key);
	
	void reset_status(void);

private:
	uint16_t m_info_type;
	std::string m_service_name;	//
	
	// Redis Server num & status
	uint32_t m_redis_server_num;
	std::map<std::string, uint32_t> m_redis_status;	// redis_id --> status	
	
	SDServerDispatcher_ptr m_dispatcher;
	SDRedisServerManager_ptr m_rs_manager;
		
	std::string m_zk_host;
	uint32_t m_zk_recv_to;
	std::string m_zk_service;

	bool m_backup_xml;
		
	boost::shared_ptr<ZKDisConf> m_dis_conf;
	
	uint32_t m_check_interval;
		
    	DECL_LOGGER(logger);

	friend class ZKDisConf;
	
};


typedef boost::shared_ptr<SDRedisStatusChecker> SDRedisStatusChecker_ptr;

#endif	// __SD_REDIS_STATUS_CHECKER_H__


