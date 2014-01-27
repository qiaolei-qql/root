#ifndef SD_DENY_SLOW_CRAWLER_H
#define SD_DENY_SLOW_CRAWLER_H

#include "SDDisCache.h"
#include "dis_cache.h"
#include <common/SDLogger.h>
#include <btqueryserver/fast_log.h>
#include <boost/thread.hpp>
#include <list>
using std::list;
using std::string;

struct SDDenySlowCrawlerValue
{	
	uint32_t m_query_times;
	uint32_t m_last_acc_time;
    list<uint32_t> m_acc_intervals;
};

class SDDenySlowCrawler : public SDDisCache
{

public:
    const static uint32_t m_sign = 123456788;

public:
	SDDenySlowCrawler();
	~SDDenySlowCrawler();
    bool queryIPValidDenyTimeOut(uint32_t ip, const string& peerid);

    bool initIPCache(
		uint32_t bucket_num,
		uint32_t critical_num,
        uint32_t max_variance_num=10,
        uint32_t max_acc_interval=3600,
        float max_acc_interval_variance=0.15,
        uint32_t stat_start_times=50);

private:		
    bool queryIP(int32_t ip, bool& exist, SDDenySlowCrawlerValue& cache_value);

    bool insertIP(int32_t ip, const SDDenySlowCrawlerValue& value);
    bool deleteIP(int32_t ip);

    std::string dump_slow_crawler_value(SDDenySlowCrawlerValue& cache_value);
    float get_acc_intervals_variance(list<uint32_t>& acc_intervals);

	static int primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);	
	static int secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null);
	static int valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg);
	static int cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg);
    static void copyValuePointorFunc(void** desc_ptr, void *src_ptr);
	
	
protected:
private:
	uint32_t m_max_variance_num;
	uint32_t m_max_acc_interval;
    float m_max_acc_interval_variance;
    uint32_t m_stat_start_times;

    boost::shared_mutex m_ds_rw_lock;
	fast_log_t * m_fltp_log;
    DECL_LOGGER(logger);
};
#endif //SD_DENY_SLOW_CRAWLER_H
