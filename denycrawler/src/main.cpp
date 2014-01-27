#include "base_header.h"
#include <iostream>
#include <boost/thread.hpp>
#include <boost/assert.hpp>
using namespace boost;
using namespace std;
int g_cnt=0;
int last_time=0;
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
    
    int cur_time = time(NULL);
    if (last_time < cur_time){
        cout <<"reset "<< last_time << " " << cur_time<<endl;
        last_time = cur_time;
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
