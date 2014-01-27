#include "SDDenySlowCrawler.h"
#include <common/SDUtility.h>
#include <iostream>
#include <string>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>

using namespace std;

IMPL_LOGGER(SDDenySlowCrawler, logger);
SDDenySlowCrawler::SDDenySlowCrawler()
{
}
SDDenySlowCrawler::~SDDenySlowCrawler()
{
}

bool SDDenySlowCrawler::initIPCache(
		uint32_t bucket_num,
		uint32_t critical_num,
        uint32_t max_variance_num,
        uint32_t max_acc_interval,
        float max_acc_interval_variance,
        uint32_t stat_start_times)
{
    m_max_variance_num = max_variance_num;
    m_max_acc_interval = max_acc_interval;
    m_max_acc_interval_variance = max_acc_interval_variance;
    m_stat_start_times = stat_start_times;
    LOG4CPLUS_DEBUG(logger,"init max_variance_num:" << max_variance_num << "max_acc_interval_variance:"<<max_acc_interval_variance
            << "max_acc_interval " << max_acc_interval);
    return initFull(
            m_sign, 
            bucket_num, 
            1,
            critical_num,
            1,
            primaryCmpFunc, 
            secondCmpFunc,
            0, 
            valueCmpFunc,
            cleanupFunc,
            copyValuePointorFunc);		
}

bool SDDenySlowCrawler::queryIPValidDenyTimeOut(uint32_t ip, const string& peerid)
{
    uint32_t ip_peerid_hash = SDUtility::get_ip_peerid_hash(ip,peerid);
    bool exist = true;
    time_t current_time = time(NULL);
    bool res = false;
    uint32_t value_len = sizeof(SDDenySlowCrawlerValue);
    SDDenySlowCrawlerValue cache_value ;
    if (queryIP(ip_peerid_hash, exist, cache_value)){
        if (!exist ){
                cache_value.m_query_times = 1;
                cache_value.m_last_acc_time = current_time;
                LOG4CPLUS_DEBUG(logger,"not exit ip:" << ip << "current_time:" << current_time << "cache_value:" << &cache_value);
                insertIP(ip_peerid_hash, cache_value);
                return true;
        }
        else{			
            uint32_t cur_acc_interval = current_time - cache_value.m_last_acc_time;
            LOG4CPLUS_DEBUG(logger,"cur_acc_interval:" << cur_acc_interval <<"current_time:"<<current_time << " last_acc_time" <<  cache_value.m_last_acc_time);
            cache_value.m_query_times ++;
            cache_value.m_last_acc_time = current_time;
            if (cur_acc_interval >= m_max_acc_interval){
                //delete this ip not a clawler
                deleteIP(ip_peerid_hash);
                return true;
            }
            if (cache_value.m_query_times > m_max_variance_num) {
                cache_value.m_acc_intervals.pop_front();
                cache_value.m_acc_intervals.push_back(cur_acc_interval);
                float acc_intervals_variance = get_acc_intervals_variance(cache_value.m_acc_intervals);
                LOG4CPLUS_DEBUG(logger,"exit ip:" << ip << "peerid:" << peerid << " acc_intervals_variance:" << acc_intervals_variance<< dump_slow_crawler_value(cache_value));
                if (cache_value.m_query_times > m_stat_start_times && acc_intervals_variance < m_max_acc_interval_variance){
                    LOG4CPLUS_INFO(logger,"deny ip:" << ip << "peerid:" << peerid << " acc_intervals_variance:" << acc_intervals_variance );
                    insertIP(ip_peerid_hash, cache_value);	
                    return false;
                }
            }
            else{
                LOG4CPLUS_DEBUG(logger, "before push back m_acc_intervals list size:" << cache_value.m_acc_intervals.size());
                cache_value.m_acc_intervals.push_back(cur_acc_interval);
                LOG4CPLUS_DEBUG(logger, "before push back m_acc_intervals list size:" << cache_value.m_acc_intervals.size());
            }
            insertIP(ip_peerid_hash, cache_value);	
            return true;
        }
    }
    return true;
}
std::string SDDenySlowCrawler::dump_slow_crawler_value(SDDenySlowCrawlerValue& cache_value)
{
    std::ostringstream oss;
    oss << " m_query_times:" <<  cache_value.m_query_times
	    << " m_last_acc_time" << cache_value.m_last_acc_time
        << " m_acc_intervals size:" << cache_value.m_acc_intervals.size();
    for (std::list<uint32_t>::iterator it=cache_value.m_acc_intervals.begin(); it != cache_value.m_acc_intervals.end(); ++it){
        oss << "item " <<*it << " ";
    }
    return oss.str();
}
bool SDDenySlowCrawler::queryIP(int32_t ip, bool& exist, SDDenySlowCrawlerValue& cache_value)
{
    LOG4CPLUS_DEBUG(logger,"query ip");
    SDDenySlowCrawlerValue* p_cache_value = NULL;
    int32_t primary_key = ip;
    int flag = m_query_cache_operation;
    void* value;
    uint32_t value_len;
    int32_t timeout_or_null_p = 0;
    boost::shared_lock<boost::shared_mutex> sl(m_ds_rw_lock);
    if (!find(
                m_sign, 
                (void*)(&primary_key),
                sizeof(primary_key),
                sizeof(primary_key),
                NULL, 
                0, 
                &value, 
                &value_len, 
                (void*)(&flag),		
                &timeout_or_null_p)){
        p_cache_value = NULL;
        LOG4CPLUS_DEBUG(logger,"query ip find error");
        return false;
    }
    if (!value || value_len == 0){
        exist = false;
    }
    else{
        exist = true;
        p_cache_value = (SDDenySlowCrawlerValue*)value;
        cache_value.m_query_times = p_cache_value->m_query_times;
        cache_value.m_last_acc_time = p_cache_value->m_last_acc_time;
        cache_value.m_acc_intervals = p_cache_value->m_acc_intervals;
    }	
    return true;	
}
bool SDDenySlowCrawler::deleteIP(int32_t ip)
{
    int32_t primary_key = ip;
    boost::unique_lock<boost::shared_mutex> ul(m_ds_rw_lock);
    if (!remove(
                m_sign, 
                (void*)(&primary_key),
                sizeof(primary_key),
                sizeof(primary_key),
                NULL, 
                0, 
                NULL))
    {
        LOG4CPLUS_ERROR(logger,"delete ip failed");
        return false;
    }
        LOG4CPLUS_DEBUG(logger,"delete ip succ: " << ip);
    return true;
}
float SDDenySlowCrawler::get_acc_intervals_variance(list<uint32_t>& acc_intervals)
{
    using namespace boost::accumulators;
    accumulator_set<uint32_t, stats<tag::variance> > acc;
    for (std::list<uint32_t>::iterator it=acc_intervals.begin(); it != acc_intervals.end(); ++it){
        acc(*it);
    }
    return variance(acc);
}
bool SDDenySlowCrawler::insertIP(int32_t ip,const  SDDenySlowCrawlerValue& value)
{
    int flag = m_update_insert_value_cache_operation;
    int32_t primary_key = ip;
    uint32_t value_len = sizeof(SDDenySlowCrawlerValue);
    SDDenySlowCrawlerValue* cache_value = new SDDenySlowCrawlerValue();
    if (!cache_value){
        LOG4CPLUS_ERROR(logger,"new SDDenySlowCrawlerValue eerror");
        return false;
    }
    cache_value->m_query_times = value.m_query_times;
    cache_value->m_last_acc_time = value.m_last_acc_time;
    cache_value->m_acc_intervals = value.m_acc_intervals;

    LOG4CPLUS_DEBUG(logger,"insert ip:" << ip << dump_slow_crawler_value(*cache_value));

    boost::unique_lock<boost::shared_mutex> ul(m_ds_rw_lock);
    if (!insertReplace(
                m_sign,
                (void*)(&primary_key),
                sizeof(primary_key),
                sizeof(primary_key),
                NULL, 
                0, 
                (void*)cache_value,
                value_len,
                (void*)(&flag), 
                NULL)){

        return false;
    }
    LOG4CPLUS_DEBUG(logger,"insert ip succ");
    return true;	
}

int SDDenySlowCrawler::primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
{
    int32_t* key1 = (int32_t*)data1;
    int32_t* key2 = (int32_t*)data2;

    if (data1_len < data2_len){
        return -1;
    }
    else if (data1_len > data2_len){
        return 1;
    }
    if (*key1 < *key2){
        return -1;
    }
    if (*key1 > *key2){
        return 1;
    }	
    return 0;	
}
int SDDenySlowCrawler::secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
{
    return 0;
}
int SDDenySlowCrawler::valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg)
{
    return 0;
}
int SDDenySlowCrawler::cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg)
{
    SDDenySlowCrawlerValue* cache_value = (SDDenySlowCrawlerValue*)data;
    if (cache_value){
        delete cache_value;
        cache_value = NULL;
    }
    return 0;
}
void SDDenySlowCrawler::copyValuePointorFunc(void** desc_ptr, void *src_ptr)
{   
    if (src_ptr == NULL){
        *desc_ptr = NULL;
        return ;
    }
    SDDenySlowCrawlerValue* desc_url_info = new SDDenySlowCrawlerValue();
    SDDenySlowCrawlerValue* src_url_info = (SDDenySlowCrawlerValue*)src_ptr;
    desc_url_info->m_query_times = src_url_info->m_query_times;
    desc_url_info->m_last_acc_time = src_url_info->m_last_acc_time;
    desc_url_info->m_acc_intervals = src_url_info->m_acc_intervals ;
    LOG4CPLUS_DEBUG(logger,"copy func list src size" <<  src_url_info->m_acc_intervals.size()<< "dst size:"<< desc_url_info->m_acc_intervals.size());

    *desc_ptr = (void*)desc_url_info;

    //LOG4CPLUS_DEBUG(logger, "url info in cache:" << src_ptr << " new url info copy:" << desc_url_info);
}

