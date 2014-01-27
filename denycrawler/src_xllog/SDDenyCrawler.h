#ifndef SD_DENY_CRAWLER_H
#define SD_DENY_CRAWLER_H
#include <stdint.h>
#include <string>
#include <common/SDLogger.h>
class SDDenySlowCrawler;
class SDBtDenySlowCrawler;
class SDBtDenyFrequentCrawler;
class SDDenyFrequentCrawler;
class SDDenyIpOverPidNumLimitCrawler;
using std::string;
class SDDenyCrawler 
{
public:
    SDDenyCrawler();
    ~SDDenyCrawler();
    void read_conf();
    bool query_valid(uint32_t ip,const string& peerid);
    bool btquery_valid(uint32_t ip,const string& peerid,const string& info_id);
    bool query_ip_peerid(uint32_t ip, std::string peerid);
private:
    DECL_LOGGER(logger);

    SDBtDenyFrequentCrawler* m_bt_deny_frequent_crawler;
    SDDenyFrequentCrawler* m_deny_frequent_crawler;

    SDDenySlowCrawler* m_deny_slow_crawler;
    SDBtDenySlowCrawler* m_bt_deny_slow_crawler;

    SDDenyIpOverPidNumLimitCrawler* m_deny_ip_over_pid_limit_crawler;
};
#endif
