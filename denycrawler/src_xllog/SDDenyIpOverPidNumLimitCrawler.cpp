#include "SDDenyIpOverPidNumLimitCrawler.h"
#include <iostream>
#include <string>

using namespace std;

IMPL_LOGGER(SDDenyIpOverPidNumLimitCrawler, logger);
SDDenyIpOverPidNumLimitCrawler::SDDenyIpOverPidNumLimitCrawler()
{
}
SDDenyIpOverPidNumLimitCrawler::~SDDenyIpOverPidNumLimitCrawler()
{
}

bool SDDenyIpOverPidNumLimitCrawler::initIPCache(
		uint32_t bucket_num,
		uint32_t critical_num,
        uint32_t max_pid_num,
        uint32_t stat_time_interval)
{
    m_fltp_log = fast_log_init("../stat/ip_over_pid_num_limit_list.log", 100, 52428800, 409600, 1, "./fast_log_error_path", 1, FAST_LOG_LEVEL_DEBUG);
    m_max_pid_num = max_pid_num;
    m_stat_time_interval = stat_time_interval;

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

bool SDDenyIpOverPidNumLimitCrawler::query_ip_peerid(uint32_t ip,string peerid)
{
    bool exist = true;
    time_t current_time = time(NULL);
    bool res = false;
    uint32_t value_len = sizeof(SDDenyIpOverPidNumLimitCrawlerValue);
    SDDenyIpOverPidNumLimitCrawlerValue cache_value ;
    if (queryIP(ip, exist, cache_value)){
        if (!exist ){
                cache_value.m_query_times = 1;
                cache_value.m_timeout = current_time;
                cache_value.m_timeout = current_time + m_stat_time_interval;
                cache_value.m_peerids.insert(peerid);
                LOG4CPLUS_DEBUG(logger,"not exit ip:" << ip << "current_time:" << current_time << "cache_value:" << &cache_value);
        }
        else{			
            LOG4CPLUS_DEBUG(logger,"exit " <<"current_time:"<<current_time << "ip" << ip << "peerid" << peerid << " " << dump_slow_crawler_value(cache_value));
            if (current_time >= cache_value.m_timeout){
                //delete this ip not a clawler
                deleteIP(ip);
                return true;
            }
            cache_value.m_query_times ++;
            std::pair<std::set<string>::iterator,bool> ret;
            ret = cache_value.m_peerids.insert(peerid);
            LOG4CPLUS_DEBUG(logger,"ip:" << ip << " list size:" << cache_value.m_peerids.size() 
                    << "insert ret:" << ret.second);
            if (cache_value.m_peerids.size()>m_max_pid_num && true==ret.second ) {
            
                char buf[1024];
                int buf_len = snprintf(buf, 1024, "ip:%d %s\n", ip,dump_slow_crawler_value(cache_value).c_str());
                fast_log_interval(m_fltp_log);
                fast_log_debug(m_fltp_log, buf, buf_len);

                LOG4CPLUS_DEBUG(logger,"pid_num_over_limit_ip:" << ip << dump_slow_crawler_value(cache_value));
            }
        }
        insertIP(ip, cache_value);	
    }
    return true;
}
std::string SDDenyIpOverPidNumLimitCrawler::dump_slow_crawler_value(SDDenyIpOverPidNumLimitCrawlerValue& cache_value)
{
    std::ostringstream oss;
    oss << " m_query_times:" <<  cache_value.m_query_times
	    << " m_timeout:" << cache_value.m_timeout
        << " m_pid_num:" << cache_value.m_peerids.size();
    return oss.str();
}
bool SDDenyIpOverPidNumLimitCrawler::queryIP(int32_t ip, bool& exist, SDDenyIpOverPidNumLimitCrawlerValue& cache_value)
{
    LOG4CPLUS_DEBUG(logger,"query ip");
    SDDenyIpOverPidNumLimitCrawlerValue* p_cache_value = NULL;
    int32_t primary_key = ip;
    int flag = m_query_cache_operation;
    void* value;
    uint32_t value_len;
    int32_t timeout_or_null_p = 0;
    boost::shared_lock<boost::shared_mutex> sl(m_iopn_rw_lock);
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
        p_cache_value = (SDDenyIpOverPidNumLimitCrawlerValue*)value;
        cache_value.m_query_times = p_cache_value->m_query_times;
        cache_value.m_timeout = p_cache_value->m_timeout;
        cache_value.m_peerids = p_cache_value->m_peerids;
    }	
    LOG4CPLUS_DEBUG(logger,"query ip end ");
    return true;	
}
bool SDDenyIpOverPidNumLimitCrawler::deleteIP(int32_t ip)
{
    int32_t primary_key = ip;
    boost::unique_lock<boost::shared_mutex> ul(m_iopn_rw_lock);
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
bool SDDenyIpOverPidNumLimitCrawler::insertIP(int32_t ip,const  SDDenyIpOverPidNumLimitCrawlerValue& value)
{
    int flag = m_update_insert_value_cache_operation;
    int32_t primary_key = ip;
    uint32_t value_len = sizeof(SDDenyIpOverPidNumLimitCrawlerValue);
    SDDenyIpOverPidNumLimitCrawlerValue* cache_value = new SDDenyIpOverPidNumLimitCrawlerValue();
    if (!cache_value){
        LOG4CPLUS_ERROR(logger,"new SDDenyIpOverPidNumLimitCrawlerValue eerror");
        return false;
    }
    cache_value->m_query_times = value.m_query_times;
    cache_value->m_timeout = value.m_timeout;
    cache_value->m_peerids = value.m_peerids;

    LOG4CPLUS_DEBUG(logger,"insert ip:" << ip << dump_slow_crawler_value(*cache_value));

    boost::unique_lock<boost::shared_mutex> ul(m_iopn_rw_lock);
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

int SDDenyIpOverPidNumLimitCrawler::primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
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
int SDDenyIpOverPidNumLimitCrawler::secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
{
    return 0;
}
int SDDenyIpOverPidNumLimitCrawler::valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg)
{
    return 0;
}
int SDDenyIpOverPidNumLimitCrawler::cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg)
{
    SDDenyIpOverPidNumLimitCrawlerValue* cache_value = (SDDenyIpOverPidNumLimitCrawlerValue*)data;
    if (cache_value){
        delete cache_value;
        cache_value = NULL;
    }
    return 0;
}
void SDDenyIpOverPidNumLimitCrawler::copyValuePointorFunc(void** desc_ptr, void *src_ptr)
{   
    if (src_ptr == NULL){
        *desc_ptr = NULL;
        return ;
    }
    SDDenyIpOverPidNumLimitCrawlerValue* desc_value = new SDDenyIpOverPidNumLimitCrawlerValue();
    SDDenyIpOverPidNumLimitCrawlerValue* src_value = (SDDenyIpOverPidNumLimitCrawlerValue*)src_ptr;
    desc_value->m_query_times = src_value->m_query_times;
    desc_value->m_timeout = src_value->m_timeout;
    desc_value->m_peerids = src_value->m_peerids;

    *desc_ptr = (void*)desc_value;
}

