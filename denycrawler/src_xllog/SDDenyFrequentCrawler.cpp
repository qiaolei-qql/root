#include "SDDenyFrequentCrawler.h"
#include <common/SDUtility.h>
#include <iostream>

using namespace std;

IMPL_LOGGER(SDDenyFrequentCrawler, logger);
SDDenyFrequentCrawler::SDDenyFrequentCrawler()
{
    //m_fltp_log = fast_log_init("../stat/ip_over_pid_num_limit_list.log", 100, 52428800, 409600, 1, "./fast_log_error_path", 1, FAST_LOG_LEVEL_DEBUG);
}
SDDenyFrequentCrawler::~SDDenyFrequentCrawler()
{
}

bool SDDenyFrequentCrawler::initIPCache(
        uint32_t bucket_num,
        uint32_t critical_num,
        uint32_t stat_time_interval,
		uint32_t query_critical_times_each_ip,
        uint32_t stat_deny_time_interval)
{
    m_stat_time_interval = stat_time_interval;
    m_query_critical_times_each_ip = query_critical_times_each_ip;
    m_stat_deny_time_interval = stat_deny_time_interval;

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
/*
bool SDDenyFrequentCrawler::queryIPValid(int ip)
{
    bool exist = true;
    uint32_t query_times = 0;
    uint32_t timeout = 0;
    time_t current_time = time(NULL);
    bool res = false;
    if (queryIP(ip, exist, query_times, timeout)){

        if (!exist || timeout <= (uint32_t)current_time){
            timeout = current_time + m_stat_time_interval;
            query_times = 1;
            res = insertIP(ip, query_times, timeout);

        }
        else{			
            query_times ++;
            res = insertIP(ip, query_times, timeout);	

        }		
    }

    if (query_times > m_query_critical_times_each_ip){
        fast_log_interval(m_fltp_log);

        const int BUF_SIZE = 256;
        char buf[BUF_SIZE];
        int buf_len = snprintf(buf, BUF_SIZE, "%d\n", ip);
        fast_log_debug(m_fltp_log, buf, buf_len);
        return false;
    }
    return true;
}
*/
bool SDDenyFrequentCrawler::queryIPValidDenyTimeOut(uint32_t ip, const string& peerid)
{
    uint32_t ip_peerid_hash = SDUtility::get_ip_peerid_hash(ip,peerid);
    bool exist = true;
    SDDenyFrequentCrawlerValue cache_value;
    time_t current_time = time(NULL);
    bool res = false;
    if (queryIP(ip_peerid_hash, exist, cache_value)){
        if (!exist){
                cache_value.m_timeout = current_time + m_stat_time_interval;
                cache_value.m_query_times = 1;
                cache_value.m_deny_timeout = 0;
                res = insertIP(ip_peerid_hash, cache_value);
        }
        else{			
            if ( ((uint32_t)current_time >= cache_value.m_timeout && cache_value.m_query_times < m_query_critical_times_each_ip) || 
                    ((uint32_t)current_time >= cache_value.m_deny_timeout && 0 != cache_value.m_deny_timeout && cache_value.m_query_times >= m_query_critical_times_each_ip) ) {

                LOG4CPLUS_DEBUG(logger,"reset ip:" << ip  << "peerid:" << peerid << cache_value.dump());
                cache_value.m_timeout = current_time + m_stat_time_interval;
                cache_value.m_query_times = 1;
                cache_value.m_deny_timeout = 0;
                res = insertIP(ip_peerid_hash,  cache_value);
            }
            else {
                cache_value.m_query_times ++;
                LOG4CPLUS_DEBUG(logger,"count ip:" << ip  << "peerid:" << peerid << cache_value.dump());
            }
            if (cache_value.m_query_times >= m_query_critical_times_each_ip && 0 == cache_value.m_deny_timeout){
                cache_value.m_deny_timeout = (uint32_t)current_time + m_stat_deny_time_interval;
            }
            res = insertIP(ip_peerid_hash,  cache_value);	
        }		
    }
    if (cache_value.m_query_times > m_query_critical_times_each_ip){
        //fast_log_interval(m_fltP_log);

        LOG4CPLUS_DEBUG(logger,"deney: ip" << ip << "peerid:" << peerid << cache_value.dump());
        return false;
    }
    return true;
}

bool SDDenyFrequentCrawler::queryIP(int32_t ip, bool& exist, SDDenyFrequentCrawlerValue& cache_value)
{
    LOG4CPLUS_DEBUG(logger,"query ip");
    SDDenyFrequentCrawlerValue* p_cache_value = NULL;
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
        p_cache_value = (SDDenyFrequentCrawlerValue*)value;
        /*cache_value.m_query_times = p_cache_value->m_query_times;
        cache_value.m_last_acc_time = p_cache_value->m_last_acc_time;
        cache_value.m_timeout = p_cache_value->m_timeout;
        cache_value.m_deny_timeout= p_cache_value->m_deny_timeout;
        cache_value.m_info_ids = p_cache_value->m_info_ids;*/
        cache_value = *p_cache_value;
    }	
    return true;	
}
/*
bool SDDenyFrequentCrawler::queryIP(int32_t ip, bool& exist, uint32_t& query_times, uint32_t& timeout)
{
    int32_t primary_key = ip;

    int flag = m_query_cache_operation;

    void* value;
    uint32_t value_len;

    int32_t timeout_or_null_p = 0;

    SDDenyFrequentCrawlerValue* cache_value = NULL;

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

        return false;
    }

    if (!value || value_len == 0){
        exist = false;
        query_times = 0;
        timeout = time(NULL) + m_stat_time_interval;
    }
    else{
        exist = true;
        cache_value = (SDDenyFrequentCrawlerValue*)value;
        query_times = cache_value->m_query_times;
        timeout = cache_value->m_timeout;
    }	
    return true;	
}

bool SDDenyFrequentCrawler::queryIP(int32_t ip, bool& exist, uint32_t& query_times, uint32_t& timeout,uint32_t& deny_timeout)
{
    int32_t primary_key = ip;

    int flag = m_query_cache_operation;

    void* value;
    uint32_t value_len;

    int32_t timeout_or_null_p = 0;

    SDDenyFrequentCrawlerValue* cache_value = NULL;

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

        return false;
    }

    if (!value || value_len == 0){
        exist = false;
        query_times = 0;
        timeout = time(NULL) + m_stat_time_interval;
        deny_timeout = 0;
    }
    else{
        exist = true;
        cache_value = (SDDenyFrequentCrawlerValue*)value;
        query_times = cache_value->m_query_times;
        timeout = cache_value->m_timeout;
        deny_timeout= cache_value->m_deny_timeout;
    }
    return true;	
}
*/
bool SDDenyFrequentCrawler::insertIP(int32_t ip,const  SDDenyFrequentCrawlerValue& value)
{
    int flag = m_update_insert_value_cache_operation;
    int32_t primary_key = ip;
    uint32_t value_len = sizeof(SDDenyFrequentCrawlerValue);
    SDDenyFrequentCrawlerValue* cache_value = new SDDenyFrequentCrawlerValue();
    if (!cache_value){
        LOG4CPLUS_ERROR(logger,"new SDDenyFrequentCrawlerValue eerror");
        return false;
    }
    /*
    cache_value->m_query_times = value.m_query_times;
    cache_value->m_timeout = value.m_timeout;
    cache_value->m_deny_timeout= value.m_deny_timeout;
    cache_value->m_last_acc_time = value.m_last_acc_time;
    cache_value->m_info_ids = value.m_info_ids;
    */
    *cache_value = value;
    LOG4CPLUS_DEBUG(logger,"insert ip:" << ip << cache_value->dump());

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
/*
bool SDDenyFrequentCrawler::insertIP(int32_t ip, uint32_t query_times, uint32_t timeout)
{
    int32_t primary_key = ip;
    uint32_t value_len = sizeof(SDDenyFrequentCrawlerValue);
    SDDenyFrequentCrawlerValue* value = new SDDenyFrequentCrawlerValue();
    value->m_query_times = query_times;
    value->m_timeout = timeout;

    int flag = m_update_insert_value_cache_operation;

    if (!insertReplace(
                m_sign,
                (void*)(&primary_key),
                sizeof(primary_key),
                sizeof(primary_key),
                NULL, 
                0, 
                (void*)value,
                value_len,
                (void*)(&flag), 
                NULL)){

        return false;
    }
    return true;	
}
bool SDDenyFrequentCrawler::insertIP(int32_t ip, uint32_t query_times, uint32_t timeout,uint32_t deny_timeout)
{
    int32_t primary_key = ip;
    uint32_t value_len = sizeof(SDDenyFrequentCrawlerValue);
    SDDenyFrequentCrawlerValue* value = new SDDenyFrequentCrawlerValue();
    if (!value) {
        LOG4CPLUS_ERROR(logger,"new SDDenyFrequentCrawlerValue eror ");
        return false;
    }

    value->m_query_times = query_times;
    value->m_timeout = timeout;
    value->m_deny_timeout = deny_timeout;

    int flag = m_update_insert_value_cache_operation;

    if (!insertReplace(
                m_sign,
                (void*)(&primary_key),
                sizeof(primary_key),
                sizeof(primary_key),
                NULL, 
                0, 
                (void*)value,
                value_len,
                (void*)(&flag), 
                NULL)){

        return false;
    }
    return true;	
}
*/
int SDDenyFrequentCrawler::primaryCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
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
int SDDenyFrequentCrawler::secondCmpFunc(void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg, int *timeout_or_null)
{
    return 0;
}
int SDDenyFrequentCrawler::valueCmpFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data1, uint32_t data1_len, void *data2, uint32_t data2_len, void *arg)
{
    return 0;
}
int SDDenyFrequentCrawler::cleanupFunc(void *primary_key, uint32_t primary_key_len, void *second_key, uint32_t second_key_len, void *data, uint32_t data_len, void *arg)
{
    SDDenyFrequentCrawlerValue* cache_value = (SDDenyFrequentCrawlerValue*)data;
    if (cache_value){
        delete cache_value;
        cache_value = NULL;
    }
    return 0;
}
void SDDenyFrequentCrawler::copyValuePointorFunc(void** desc_ptr, void *src_ptr)
{   
    if (src_ptr == NULL){
        *desc_ptr = NULL;
        return ;
    }
    SDDenyFrequentCrawlerValue* desc_url_info = new SDDenyFrequentCrawlerValue();
    SDDenyFrequentCrawlerValue* src_url_info = (SDDenyFrequentCrawlerValue*)src_ptr;
    desc_url_info->m_timeout = src_url_info->m_timeout;
    desc_url_info->m_query_times = src_url_info->m_query_times;
    desc_url_info->m_deny_timeout = src_url_info->m_deny_timeout;
    *desc_ptr = (void*)desc_url_info;
}

