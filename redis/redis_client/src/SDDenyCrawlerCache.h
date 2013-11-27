#ifndef SD_DENY_CRAWLER_CACHE_H
#define SD_DENY_CRAWLER_CACHE_H

#include "base_header.h"
#include <boost/thread.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
// txt
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
//for binay
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
//for xml
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
//for stl
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include "SDRedisServerPool.h"

struct SDBtDenyCrawlerKey
{
    uint32_t m_ip;
    string m_peerid;
    string info_id;
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
#ifdef DEBUG_XML_SERIALIZE
            ar & BOOST_SERIALIZATION_NVP(m_ip);
            ar & BOOST_SERIALIZATION_NVP(m_peerid);
#else
            ar & m_ip;
            ar & m_peerid;
#endif
        }
    string dump()const {
        std::ostringstream oss;
        oss << " m_ip:" <<  m_ip
            << " m_peerid:" << m_peerid
            << " info_id:" << info_id;
        return oss.str();
    }
};
struct SDDenyCrawlerKey
{
    uint32_t m_ip;
    string m_peerid;
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
#ifdef DEBUG_XML_SERIALIZE
            ar & BOOST_SERIALIZATION_NVP(m_ip);
            ar & BOOST_SERIALIZATION_NVP(m_peerid);
#else
            ar & m_ip;
            ar & m_peerid;
#endif
        }
    string dump()const {
        std::ostringstream oss;
        oss << " m_ip:" <<  m_ip
            << " m_peerid:" << m_peerid;
        return oss.str();
    }
};

template<typename RedisKey,typename RedisValue>
class SDDenyCrawlerCache
{

    public:
        SDDenyCrawlerCache();
        ~SDDenyCrawlerCache();
        bool init(const std::string& host, uint16_t port, uint16_t db_index,uint32_t key_update_expire);
        //bool queryIPValid(int ip);
        //bool query_valid(const RedisKey& key, const RedisValue& value);
        bool get(const RedisKey& key, bool& exist, RedisValue& value);
        bool set(const RedisKey& key,const RedisValue& value);
        bool del(const RedisKey& key);
        string serialize(const RedisKey& key);
        string serialize(const RedisValue& val);
        bool deserialize(const string &strkey,RedisKey& key);
        bool deserialize(const string &strval,RedisValue& val);

        //bool query_valid(const RedisKey& key);
        //virtual bool query_ip_peerid();

    private:
        uint32_t m_key_update_expire; 
        SDRedisServerPool_ptr m_rs_pool;
        boost::shared_mutex m_ds_rw_lock;
        DECL_LOGGER(logger);
};
#include "SDDenyCrawlerCache.cpp"
#endif //SD_DENY_CRAWLER_H
