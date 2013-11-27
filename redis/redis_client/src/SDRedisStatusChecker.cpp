#include "SDRedisStatusChecker.h"
#include "SDRedisServerManager.h"
#include "SDFailoverProxy.h"
#include "SDConfigurationSingleton.h"
#include "common/SDUtility.h"
#include <errno.h>
#include <assert.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


using namespace std;

IMPL_LOGGER(SDRedisStatusChecker, logger);


SDRedisStatusChecker::SDRedisStatusChecker(uint16_t info_type, std::string service_name)
:m_info_type(info_type), m_service_name(service_name), m_redis_server_num(0)
{
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
	reset_status();	
}

SDRedisStatusChecker::~SDRedisStatusChecker()
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
}

bool SDRedisStatusChecker::init(SDServerDispatcher_ptr dispatcher, SDRedisServerManager_ptr rs_manager)
{
	LOG4CPLUS_DEBUG(logger, "SDRedisStatusChecker start init");

	m_dispatcher = dispatcher;
	m_rs_manager = rs_manager;
		
	SDConfiguration* config = SDConfigurationSingleton::get_instance()->get_config();

	m_zk_host = config->getString("status.zk.server.host.port");
	m_zk_recv_to =  config->getInt("status.zk.session.timeout",3000);
	m_zk_service = config->getString("status.zk.service","redis_status");
	
	m_check_interval = config->getInt("status.manager.check.interval.s",5);

	m_backup_xml = true;

	std::string root_key;

    //为bcid做特化处理
 #ifdef SPECIAL_BCID
    if(m_service_name=="bcid_info_cache")
    {
        char host_name[128];
		gethostname(host_name, sizeof(host_name));
        root_key = m_zk_service +"." + m_service_name+"."+std::string(host_name);
    }
    else
#endif

    {
        root_key = m_zk_service + "." + m_service_name;
    }
    
	m_dis_conf = boost::shared_ptr<ZKDisConf>(new ZKDisConf(m_zk_host, m_zk_recv_to, root_key, m_backup_xml));
	if(!m_dis_conf->init())
	{
		LOG4CPLUS_ERROR(logger, "ZKDisConf " << m_dis_conf << " init failed");
		return false;
	}
    
	// register callback func when redis status change
	m_dis_conf->register_update_cb(boost::bind(&SDRedisStatusChecker::sync_value, this, _1, _3));

    // make sure the node has create
    std::string nothing;
    
    m_dis_conf->set_node_not_exist(root_key,nothing,true);
    
	// read from zk(construct local pt tree)
	m_dis_conf->read_zk();
	
	LOG4CPLUS_DEBUG(logger, "ZKDisConf  " << m_dis_conf << " init succ!"); 
	return true;	
}


bool SDRedisStatusChecker::add_redis_server(const std::string &redis_id)
{
	uint32_t last_status = REDIS_UNKNOWN;
		
	if(m_redis_status.find(redis_id) != m_redis_status.end())
	{
		LOG4CPLUS_DEBUG(logger, "redis_id:" << redis_id << " has been in status manager");
		return false;
	}

	LOG4CPLUS_DEBUG(logger, "add new redis_id:" << redis_id << ",status:" << last_status << " into status map");
	m_redis_status[redis_id] = last_status;		// default status
	++m_redis_server_num;
    
	uint32_t status = check_redis_status(redis_id);
	LOG4CPLUS_DEBUG(logger, "SDRedisStatusManager add redis_id:" << redis_id << " last_status/status:" << last_status << "/" << status);

    /****************************************************************/

    std::string key,unknown("UNKNOWN");

    idtokey(redis_id, key);

    m_dis_conf->set_node_not_exist(key,unknown,true);
    
    /****************************************************************/
    LOG4CPLUS_DEBUG(logger, "force to report redis_id:" << redis_id << " status:" << (status == REDIS_LIVE ? "LIVE" : "DEAD") << " to zookeeper" );		
	if(!report(const_cast<std::string &>(redis_id), status , true))	//force flush current status to zk
	{
		LOG4CPLUS_WARN(logger, "force to report redis_id:" << redis_id << " current status:" << status << " to zk failed");
		return false;
	}
	LOG4CPLUS_DEBUG(logger, "report redis_id:" << redis_id << " current status:" << status << " to zk ok");
	
	return true;
}


bool SDRedisStatusChecker::del_redis_server(const std::string &redis_id)
{
	bool succ = m_redis_status.erase(redis_id) >0 ? true : false;
	if(succ)
	{
		--m_redis_server_num;	
	}

	LOG4CPLUS_DEBUG(logger, "del redis_id:" << redis_id << " from status map, ret:" << succ);
	
	return succ;
}


void SDRedisStatusChecker::routine()
{	
	LOG4CPLUS_DEBUG(logger, "thread id: "<<pthread_self()<<" start to do check " << m_service_name << " 's redises status...:)");

	std::map<std::string,uint32_t>::iterator it;
	for(it = m_redis_status.begin(); it != m_redis_status.end(); ++it)
	{
		std::string redis_id = it->first;
		
		uint32_t status = check_redis_status(redis_id);

		LOG4CPLUS_DEBUG(logger, "ready to report redis_id:" << redis_id << ",status:" << (status == REDIS_LIVE ? "LIVE" : "DEAD") << " to zookeeper" );			
		if(!report(redis_id, status, false))
		{
			LOG4CPLUS_WARN(logger, "report redis_id:" << redis_id << " current status:" << status << " to zk failed");
			continue;
		}
		LOG4CPLUS_DEBUG(logger, "report redis_id:" << redis_id << " current status:" << status << " to zk ok");

	}
}


bool SDRedisStatusChecker::notify(const std::string &redis_id, uint32_t status, uint32_t last_status)
{
	LOG4CPLUS_DEBUG(logger, "notify redis_id:" << redis_id << " status:" << status << " last_status:" << last_status);
		
	bool succ = false;
	
	// new added redis server
	if(REDIS_UNKNOWN == last_status)
	{
		/*  
		LOG4CPLUS_DEBUG(logger, "ready to add new redis_id:" << redis_id << ", into dispathcer's total server");
		succ = m_dispatcher->add_server(SDServerDispatcher::TOTAL, redis_id);
		*/
		
		if(REDIS_LIVE == status)
		{
			LOG4CPLUS_DEBUG(logger, "ready to add new redis_id:" << redis_id<<"info type:"<<m_info_type<< ", into dispathcer's live server");
			succ = m_dispatcher->add_server(SDServerDispatcher::LIVE, redis_id);	
		}
	}
	else
	{
		// redis relive
		if(REDIS_LIVE == status && REDIS_DEAD == last_status)
		{
			LOG4CPLUS_WARN(logger, "redis_id:" << redis_id << "info type:"<<m_info_type<<" is relive, report to failover (and dispatcher should be notified by failover after restoring)");
			succ = SDFailoverProxy::get_instance()->restore_coherence(m_info_type, redis_id);		// HSHA pattern
			//succ = m_dispatcher->add_redis(SDRedisDispatcher::LIVE, id);
		}

		// redis down
		if(REDIS_DEAD == status && REDIS_LIVE == last_status)
		{
			LOG4CPLUS_WARN(logger, "redis_id:" << redis_id << "info type:"<<m_info_type<<" is down, report to backupserver & dispatcher");
			SDFailoverProxy::get_instance()->report_down(m_info_type, redis_id);					// HSHA pattern
			succ = m_dispatcher->del_server(SDServerDispatcher::LIVE, redis_id);	
		}
	}

	return succ;
}


uint32_t SDRedisStatusChecker::check_redis_status(const std::string &redis_id)
{
	uint32_t status = REDIS_UNKNOWN;		// default status
	bool reconnect = false;				// reconnect happended?

	LOG4CPLUS_DEBUG(logger,"ready tp check redis_id:" << redis_id << " 's status");
	SDAutoRedis auto_redis = m_rs_manager->get_redis(m_info_type, redis_id);
	SDRedisServer_ptr redis_ptr = auto_redis.ptr();
	
	LOG4CPLUS_ERROR(logger, "ready to check " << redis_ptr->dump() << " status");
	
	bool succ = false;
	if(!redis_ptr->is_connected())
	{
		succ = redis_ptr->connect(2);
		if(!succ)
		{
			LOG4CPLUS_ERROR(logger, "try to connect to " << redis_ptr->dump() << " fail, this redis server is DEAD!");
			status = REDIS_DEAD;
			return status;
		}
		else
		{
			LOG4CPLUS_DEBUG(logger, "try to connect to " << redis_ptr->dump() << " succ, ready to ping redis server");
			reconnect = true;
		}
	}
	
	// ping the dest redis server
	succ = redis_ptr->ping();
	if(succ)
	{
		LOG4CPLUS_DEBUG(logger, "ping redis server to " << redis_ptr->dump() << " succ, this redis server is LIVE!");
		status = REDIS_LIVE;
	}
	else
	{
		LOG4CPLUS_ERROR(logger, "ping redis server to " << redis_ptr->dump()<< " fail, this redis server is DEAD!");
		status = REDIS_DEAD;
	}

	return status;
}


bool  SDRedisStatusChecker::report(std::string &redis_id, uint32_t status, bool force)
{
	std::string rs_zk_status =  (status == REDIS_LIVE ? "LIVE" : "DEAD");

	std::string key;
	idtokey(redis_id, key);
	
	LOG4CPLUS_DEBUG(logger, "report key:" << key << " status:" << rs_zk_status << " to zookeeper" );
    
	return m_dis_conf->set(key, rs_zk_status, force);	
}


bool SDRedisStatusChecker::sync_value(const std::string &key, std::string value)
{
	LOG4CPLUS_DEBUG(logger, "zookeeper sync:" << key << " 's value:" << value);
    
    if(!boost::contains(key, m_service_name))
	{
		LOG4CPLUS_WARN(logger, m_service_name << " uncared zookeeper sync:" << key << " 's value:" << value);
		return true;
	}

	std::string redis_id;
	
	keytoid(key, redis_id);
	
	std::map<std::string,uint32_t>::iterator it = m_redis_status.find(redis_id);
	if(it == m_redis_status.end())
	{
		LOG4CPLUS_ERROR(logger, "sync redis_id:" << redis_id << " is not in redis status map");
		return false;
	}

	uint32_t status;
    
	("LIVE" == value) ? (status = REDIS_LIVE) : (status = REDIS_DEAD);
	
	uint32_t last_status = m_redis_status[redis_id];

	LOG4CPLUS_DEBUG(logger, "zookeeper sync status, update redis_id:" << redis_id << " 's local status/last_status:" << status << "/" << last_status);	

    m_redis_status[redis_id] = status;
	
	LOG4CPLUS_DEBUG(logger, "sync redis_id:" << redis_id << " ' status and notify dispatcher/failover");
	notify(redis_id, status, last_status);

	return true;
}


bool SDRedisStatusChecker::keytoid(const std::string &key, std::string &redis_id)
{
	try
	{
		std::vector<std::string> parts;
		boost::split(parts, key, boost::is_any_of("."),  boost::token_compress_off);
		BOOST_ASSERT(parts[0] == m_zk_service);
		BOOST_ASSERT(parts[1] == m_service_name);

#ifdef SPECIAL_BCID
        if(m_service_name=="bcid_info_cache")
        {
            redis_id = parts[3];
        }
        else
#endif			
        {
		    redis_id = parts[2];
        }
	}
	catch(...)
	{
		LOG4CPLUS_WARN(logger, " key:" << key << "-->" << " service_name:" << m_service_name << ",redis_id:" << redis_id);
		return false;
	}

	LOG4CPLUS_DEBUG(logger, " key:" << key << "-->" << " m_service_name:" << m_service_name << ",redis_id:" << redis_id);
	return true;
}


// key format XX.OO.FF.KK
bool SDRedisStatusChecker::idtokey(const std::string &redis_id, std::string &key)
{

#ifdef SPECIAL_BCID    
    if(m_service_name=="bcid_info_cache")
    {
        char host_name[128];
		gethostname(host_name, sizeof(host_name));
        key =  m_zk_service + "." + m_service_name + "." + std::string(host_name)+ "." + redis_id;
    }
    else
#endif
    {
	    key =  m_zk_service + "." + m_service_name + "." + redis_id;
    }
	LOG4CPLUS_DEBUG(logger,"service_name:" << m_service_name << ",redis_id:" << redis_id << " -- > " << " key:" << key);
	
	return true;
}

	
void SDRedisStatusChecker::reset_status(void)
{

}


std::string SDRedisStatusChecker::get_statistic(int reset)
{
	stringstream output;
 
	if(reset)
	{
		reset_status();
	}
	
	return output.str();
}

bool SDRedisStatusChecker::node_change(const std::string &key)
{
   
	return true;
}

