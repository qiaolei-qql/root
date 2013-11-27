#include "base_header.h"
#include "SDRedisServerPoolSingleton.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/assert.hpp>
#include "SDDenyCrawlerCache.h"
#include "SDRedisStringHandler.h"

struct SDDenyIpOverPidNumLimitCrawlerValue
{	
	uint32_t m_query_times;
	uint32_t m_timeout;
    set<string> m_peerids;
    friend class boost::serialization::access;
    template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
#ifdef DEBUG_XML_SERIALIZE
            ar & BOOST_SERIALIZATION_NVP(m_query_times);
            ar & BOOST_SERIALIZATION_NVP(m_timeout);
            ar & BOOST_SERIALIZATION_NVP(m_peerids);
#else
            ar & m_query_times;
            ar & m_timeout;
            ar & m_peerids;
#endif
        }
};

typedef boost::shared_ptr<SDDenyCrawlerCache<SDDenyCrawlerKey,SDDenyIpOverPidNumLimitCrawlerValue> >  SDDenyIpOverPidNumLimitCrawlerCache_ptr;

using namespace std;
int g_cnt=0;
int cur_time=0;
void run()
{
    while(true){
    /*SDDenyIpOverPidNumLimitCrawlerCache_ptr m_deny_crawler_cache = SDDenyIpOverPidNumLimitCrawlerCache_ptr(new SDDenyCrawlerCache<SDDenyCrawlerKey,SDDenyIpOverPidNumLimitCrawlerValue>());
    SDDenyCrawlerKey key={11111111,"peerid"};
    SDDenyIpOverPidNumLimitCrawlerValue cache_value ;
    m_deny_crawler_cache->init("127.0.0.1",6381,0);
    bool exist;
    m_deny_crawler_cache->get(key, exist, cache_value);
    */
    if (cur_time > time(NULL)){
        g_cnt =0;
    }
    g_cnt ++;
    cout << g_cnt<<endl;
    }

}

int main(){
#ifdef LOGGER
    PropertyConfigurator::doConfigure("../conf/xl_log4cplus.properties");
#endif
    //SDRedisServerPoolSingleton::get_instance()->init("127.0.0.1",6379,0);
    thread_group tg;
    tg.create_thread(run);
    tg.create_thread(run);
    tg.create_thread(run);
    tg.join_all();

   //SDRedisHandler_ptr rs_conn_handler = SDRedisHandler_ptr(new SDRedisHandler(SDRedisServer_ptr));
    return 0;
}
