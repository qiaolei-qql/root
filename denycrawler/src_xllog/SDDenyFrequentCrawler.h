#ifndef SD_DENY_FREQUENT_CRAWLER_H
#define SD_DENY_FREQUENT_CRAWLER_H

#include "SDDisCache.h"
#include "dis_cache.h"
#include <common/SDLogger.h>
#include <btqueryserver/fast_log.h>
#include <boost/thread.hpp>
#include <set>
#include <string>
using std::set;
using std::string;
struct SDDenyFrequentCrawlerValue
{	
	uint32_t m_timeout;	
    uint32_t m_deny_timeout;
	uint32_t m_query_times;
    std::string dump()
    {
        std::ostringstream oss;
        oss << " m_query_times:" <<  m_query_times
            << " m_deny_timeout :" << m_deny_timeout
            << " current_time:" << time(NULL)
            << " m_timeout: " << m_timeout;
        return oss.str();
    }
};

class SDDenyFrequentCrawler : public SDDisCache
{

public:
    const static uint32_t m_sign = 123456789;

public:
	SDDenyFrequentCrawler();
	~SDDenyFrequentCrawler();
	//bool queryIPValid(int ip);
    bool queryIPValidDenyTimeOut(uint32_t ip, const string& peerid);

	bool initIPCache(	
		uint32_t bucket_num,
		uint32_t critical_num,
		uint32_t stat_time_interval,
		uint32_t query_critical_times_each_ip,
        uint32_t stat_deny_time_interval =0);

private:		
    bool queryIP(int32_t ip, bool& exist, SDDenyFrequentCrawlerValue& cache_value);
    bool insertIP(int32_t ip,const  SDDenyFrequentCrawlerValue& value);

	/*bool queryIP(int32_t ip, bool& exist, uint32_t& query_times, uint32_t& timeout);
    bool queryIP(int32_t ip, bool& exist, uint32_t& query_times, uint32_t& timeout,uint32_t& deny_timeout,uint32_t last_acc_time , set<string>& info_ids);
	bool insertIP(int32_t ip, uint32_t query_times, uint32_t timeout);
    bool insertIP(int32_t ip, uint32_t query_times, uint32_t timeout,uint32_t deny_timeout,uint32_t last_acc_time, const  set<string>& info_ids);
    */

	static int primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);	
	static int secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);
	static int valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg);
	static int cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg);
    static void copyValuePointorFunc(void** desc_ptr, void *src_ptr);
	
	
protected:
private:
	uint32_t m_stat_time_interval;	
	uint32_t m_stat_deny_time_interval;	
	uint32_t m_query_critical_times_each_ip;

    boost::shared_mutex m_ds_rw_lock;
	fast_log_t * m_fltp_log;
    DECL_LOGGER(logger);
};
#endif //SD_DENY_FREQUENT_CRAWLER_H
