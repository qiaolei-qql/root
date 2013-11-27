#ifndef SD_DENY_CRAWLER_CPP
#define SD_DENY_CRAWLER_CPP

#include "SDDenyCrawlerCache.h"
#include <common/SDUtility.h>

#include "SDRedisStringHandler.h" 

template<typename RedisKey, typename RedisValue>
//IMPL_LOGGER(SDDenyCrawlerCache<RedisKey,RedisValue> , logger);
log4cplus::Logger SDDenyCrawlerCache<RedisKey,RedisValue>::logger = log4cplus::Logger::getInstance("SDDenyCrawlerCache<#RedisKey,#RedisValue>");


    template<typename RedisKey, typename RedisValue>
SDDenyCrawlerCache<RedisKey,RedisValue>::SDDenyCrawlerCache()
{
}
    template<typename RedisKey, typename RedisValue>
SDDenyCrawlerCache<RedisKey,RedisValue>::~SDDenyCrawlerCache()
{
}

    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::init(const std::string& host, uint16_t port, uint16_t db_index,uint32_t key_update_expire)
{
    m_key_update_expire = key_update_expire;
    m_rs_pool = SDRedisServerPool_ptr(new SDRedisServerPool(host,port,db_index));
    BOOST_ASSERT(m_rs_pool && "new m_rs_pool error");
}
/*    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::query_valid(const RedisKey& key)
{
    LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
    return true;
}
*/

    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::get(const RedisKey& key, bool& exist, RedisValue& value)
{
    //boost::shared_lock<boost::shared_mutex> sl(m_ds_rw_lock);
    string strkey = serialize(key);
    string strval;
    SDRedisStringHandler rs_hander = SDRedisStringHandler(m_rs_pool->get(),m_key_update_expire,m_key_update_expire,m_key_update_expire);
    int ret = rs_hander.str_get(strkey,strval);
    if (-1 == ret)
    {
        return false;
    }
    0 == ret ? exist = false: exist = true;
    LOG4CPLUS_DEBUG(logger,"get exit:" << exist);
    try {
        deserialize(strval,value);
    }
    catch (const boost::archive::archive_exception& e)
    {
        LOG4CPLUS_ERROR(logger,"boost::archive::archive_exception : " << e.what());
        rs_hander.str_remove(strkey);
        return false;
    }
    return true;

}
    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::set(const RedisKey& key,const RedisValue& value)
{
    //boost::unique_lock<boost::shared_mutex> ul(m_ds_rw_lock);
    string strkey = "";
    string strval = ""; 
    try {
        strkey = serialize(key);
        strval = serialize(value);
    }
    catch (const boost::archive::archive_exception& e)
    {
        LOG4CPLUS_ERROR(logger,"boost::archive::archive_exception : " << e.what());
        return false;
    }
    SDRedisStringHandler rs_hander = SDRedisStringHandler(m_rs_pool->get(),m_key_update_expire,m_key_update_expire,m_key_update_expire);
    return rs_hander.str_set(strkey,strval);
}

    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::del(const RedisKey& key)
{
    //boost::unique_lock<boost::shared_mutex> ul(m_ds_rw_lock);
    string strkey = serialize(key);
    SDRedisStringHandler rs_hander = SDRedisStringHandler(m_rs_pool->get(),m_key_update_expire,m_key_update_expire,m_key_update_expire);
    return rs_hander.str_remove(strkey);
}

    template<typename RedisKey, typename RedisValue>
string SDDenyCrawlerCache<RedisKey,RedisValue>::serialize(const RedisKey& key)
{
    stringstream sstream;
#ifdef DEBUG_XML_SERIALIZE
    boost::archive::xml_oarchive oa(sstream);
    oa << BOOST_SERIALIZATION_NVP(key);
#else
    boost::archive::binary_oarchive oa(sstream);
    oa << key;
#endif
    LOG4CPLUS_DEBUG(logger,"serialize key:" << key.dump() << "strkey: " <<sstream.str());
    return sstream.str();
}
    template<typename RedisKey, typename RedisValue>
string SDDenyCrawlerCache<RedisKey,RedisValue>::serialize(const RedisValue& val)
{
    stringstream sstream;
#ifdef DEBUG_XML_SERIALIZE
    boost::archive::xml_oarchive oa(sstream);
    oa << BOOST_SERIALIZATION_NVP(val);
#else
    boost::archive::binary_oarchive oa(sstream);
    oa << val;
#endif
    LOG4CPLUS_DEBUG(logger,"serialize val" <<sstream.str());
    return sstream.str();
}
    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::deserialize(const string& strkey,RedisKey& key)
{
    LOG4CPLUS_DEBUG(logger,"deserialize key:" << strkey );
    if (strkey == "") return true;
    stringstream sstream(strkey);
#ifdef DEBUG_XML_SERIALIZE
    boost::archive::xml_iarchive ia(sstream);
    ia >> BOOST_SERIALIZATION_NVP(key);
#else
    boost::archive::binary_iarchive ia(sstream);
    ia >> key;
#endif
    LOG4CPLUS_DEBUG(logger,"deserialize key:" << strkey );
}
    template<typename RedisKey, typename RedisValue>
bool SDDenyCrawlerCache<RedisKey,RedisValue>::deserialize(const string& strval,RedisValue& val)
{
    LOG4CPLUS_DEBUG(logger,"deserialize val:" << strval);
    if (strval == "") return true;
    stringstream sstream(strval);
#ifdef DEBUG_XML_SERIALIZE
    boost::archive::xml_iarchive ia(sstream);
    ia >> BOOST_SERIALIZATION_NVP(val);
#else
    boost::archive::binary_iarchive ia(sstream);
    ia >> val;
#endif
    LOG4CPLUS_DEBUG(logger,"deserialize val:" << strval);
}
#endif 
