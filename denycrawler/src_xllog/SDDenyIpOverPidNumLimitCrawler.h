#ifndef SD_DENY_IP_OVER_LIMIT_CRAWLER_H
#define SD_DENY_IP_OVER_LIMIT_CRAWLER_H

#include "SDDisCache.h"
#include "dis_cache.h"
#include <common/SDLogger.h>
#include <btqueryserver/fast_log.h>
#include <boost/thread.hpp>
#include <set>
using std::set;
using std::string;

struct SDDenyIpOverPidNumLimitCrawlerValue
{	
	uint32_t m_query_times;
	uint32_t m_timeout;
    set<string> m_peerids;
};

class SDDenyIpOverPidNumLimitCrawler : public SDDisCache
{

public:
    const static uint32_t m_sign = 123456787;

public:
	SDDenyIpOverPidNumLimitCrawler();
	~SDDenyIpOverPidNumLimitCrawler();
    bool query_ip_peerid(uint32_t ip,string peerid);

    bool initIPCache(
		uint32_t bucket_num,
		uint32_t critical_num,
        uint32_t max_pid_num = 7000,
        uint32_t stat_time_interval=3600);

private:		
    bool queryIP(int32_t ip, bool& exist, SDDenyIpOverPidNumLimitCrawlerValue& cache_value);

    bool insertIP(int32_t ip, const SDDenyIpOverPidNumLimitCrawlerValue& value);
    bool deleteIP(int32_t ip);

    std::string dump_slow_crawler_value(SDDenyIpOverPidNumLimitCrawlerValue& cache_value);

	static int primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);	
	static int secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);
	static int valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg);
	static int cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg);
    static void copyValuePointorFunc(void** desc_ptr, void *src_ptr);
	
	
protected:
private:
	uint32_t m_max_pid_num;
	uint32_t m_stat_time_interval;	
    boost::shared_mutex m_iopn_rw_lock;

	fast_log_t * m_fltp_log;
    DECL_LOGGER(logger);
};
#endif //SD_DENY_IP_OVER_LIMIT_CRAWLER_H
