#include "SDDenyCrawler.h"
#include "SDDenySlowCrawler.h"
#include "SDBtDenySlowCrawler.h"
#include "SDBtDenyFrequentCrawler.h"
#include "SDDenyFrequentCrawler.h"
#include "SDDenyIpOverPidNumLimitCrawler.h"
#include <common/SDConfiguration.h>
#include "btqueryserver/SDConfigurationSingleton.h"

IMPL_LOGGER(SDDenyCrawler, logger);

SDDenyCrawler::SDDenyCrawler()
:m_deny_frequent_crawler(NULL),m_deny_slow_crawler(NULL),m_deny_ip_over_pid_limit_crawler(NULL)
{
}
SDDenyCrawler::~SDDenyCrawler()
{
    if (m_deny_slow_crawler){
        delete m_deny_slow_crawler;
    }
    if (m_bt_deny_slow_crawler){
        delete m_bt_deny_slow_crawler;
    }
    if (m_deny_frequent_crawler){
        delete m_deny_frequent_crawler;
    }
    if (m_bt_deny_frequent_crawler){
        delete m_bt_deny_frequent_crawler;
    }
    if (m_deny_ip_over_pid_limit_crawler){
        delete m_deny_ip_over_pid_limit_crawler;
    }
}

void SDDenyCrawler::read_conf()
{
    const SDConfiguration& config = SDConfigurationSingleton::get_instance()->get_config();
	int ip_cache_bucket_num = atoi(config.getString("server.ip.cache.bucket.num").c_str());
	int ip_cache_critical_num = atoi(config.getString("server.ip.cache.critical.num").c_str());
	int ip_cache_time_interval = atoi(config.getString("server.ip.cache.time.interval").c_str());
	int ip_cache_max_times = atoi(config.getString("server.ip.cache.max.times").c_str());
	int ip_cache_deny_time_interval = atoi(config.getString("server.ip.cache.denytime.interval").c_str());
    m_bt_deny_frequent_crawler = new SDBtDenyFrequentCrawler();
    m_deny_frequent_crawler = new SDDenyFrequentCrawler();
    if (!m_bt_deny_frequent_crawler){
           LOG4CPLUS_ERROR(logger,"new m_bt_deny_frequent_crawler error:"<< m_deny_frequent_crawler);
           return ;
    }
    if (!m_deny_frequent_crawler){
           LOG4CPLUS_ERROR(logger,"new m_deny_frequent_crawler error:"<< m_deny_frequent_crawler);
           return ;
    }
	m_bt_deny_frequent_crawler->initIPCache(ip_cache_bucket_num, ip_cache_critical_num, ip_cache_time_interval, ip_cache_max_times,ip_cache_deny_time_interval);
	m_deny_frequent_crawler->initIPCache(ip_cache_bucket_num, ip_cache_critical_num, ip_cache_time_interval, ip_cache_max_times,ip_cache_deny_time_interval);

    int max_variance_num = atoi(config.getString("server.ip.cache.max.variance.num").c_str());
    int max_acc_interval = atoi(config.getString("server.ip.cache.max.acc.interval").c_str());
    int stat_start_times = atoi(config.getString("server.ip.cache.stat.start.times").c_str());
    float max_acc_interval_variance = (float)atof(config.getString("server.ip.cache.max.acc.interval.variance").c_str());
    m_deny_slow_crawler = new SDDenySlowCrawler();
    m_bt_deny_slow_crawler = new SDBtDenySlowCrawler();
    if (!m_deny_slow_crawler){
           LOG4CPLUS_ERROR(logger,"new m_deny_slow_crawler error:"<< m_deny_slow_crawler);
           return;
    }
    if (!m_bt_deny_slow_crawler){
           LOG4CPLUS_ERROR(logger,"new m_bt_deny_slow_crawler error:"<< m_deny_slow_crawler);
           return;
    }
    m_deny_slow_crawler->initIPCache(
            ip_cache_bucket_num, ip_cache_critical_num, max_variance_num, max_acc_interval, max_acc_interval_variance, stat_start_times);
    m_bt_deny_slow_crawler->initIPCache(
            ip_cache_bucket_num, ip_cache_critical_num, max_variance_num, max_acc_interval, max_acc_interval_variance, stat_start_times);

    int max_pid_num = atoi(config.getString("server.ip.cache.max.pid.num").c_str());
    int pid_stat_time_interval = atoi(config.getString("server.ip.cache.pid.stat.time.interval").c_str());
    m_deny_ip_over_pid_limit_crawler = new SDDenyIpOverPidNumLimitCrawler();
    if (!m_deny_ip_over_pid_limit_crawler){
           LOG4CPLUS_ERROR(logger,"new m_deny_ip_over_pid_limit_crawler error:"<< m_deny_ip_over_pid_limit_crawler);
           return;
    }
    m_deny_ip_over_pid_limit_crawler->initIPCache(ip_cache_bucket_num,ip_cache_critical_num,max_pid_num,pid_stat_time_interval);

}

bool SDDenyCrawler::query_valid(uint32_t ip,const string& peerid)
{
    bool ret1 = m_deny_frequent_crawler->queryIPValidDenyTimeOut(ip,peerid);
    LOG4CPLUS_DEBUG(logger,"m_deny_frequent_crawler queryIPValidDenyTimeOut(): "<< ret1);
    bool ret2 = m_deny_slow_crawler->queryIPValidDenyTimeOut(ip,peerid);
    LOG4CPLUS_DEBUG(logger,"m_deny_slow_crawler queryIPValidDenyTimeOut(): "<< ret2);
    return ((ret1 && ret2) ? true : false);
}
bool SDDenyCrawler::btquery_valid(uint32_t ip,const string& peerid,const string& info_id)
{
    bool ret1 = m_bt_deny_frequent_crawler->queryIPValidDenyTimeOut(ip,peerid,info_id);
    LOG4CPLUS_DEBUG(logger,"m_deny_frequent_crawler queryIPValidDenyTimeOut(): "<< ret1);
    bool ret2 = m_bt_deny_slow_crawler->queryIPValidDenyTimeOut(ip,peerid,info_id);
    LOG4CPLUS_DEBUG(logger,"m_deny_slow_crawler queryIPValidDenyTimeOut(): "<< ret2);
    return ((ret1 && ret2) ? true : false);
}
bool SDDenyCrawler::query_ip_peerid(uint32_t ip, std::string peerid)
{
    LOG4CPLUS_DEBUG(logger," query ip peerid:" << ip << " " << peerid);
    m_deny_ip_over_pid_limit_crawler->query_ip_peerid(ip,peerid);
    return true;
}

