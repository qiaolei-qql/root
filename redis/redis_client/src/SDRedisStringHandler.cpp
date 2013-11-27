#include "SDRedisStringHandler.h"
#include "common/SDConfiguration.h"
#include <btqueryserver/SDConfigurationSingleton.h>
#include "common/SDUtility.h"
#include <errno.h>



using namespace std;

IMPL_LOGGER(SDRedisStringHandler, logger);


SDRedisStringHandler::SDRedisStringHandler(SDRedisServer_ptr redis_server, uint32_t update_key_tos, uint32_t set_key_tos,uint32_t empty_key_tos)
:SDRedisHandler(redis_server, update_key_tos, set_key_tos,empty_key_tos)
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
    reset_status();	
}


SDRedisStringHandler::~SDRedisStringHandler()
{	
	LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
}

int SDRedisStringHandler::str_get(const string& key,string& value)
{
    CHECK_CONN("SDRedisStringHandler","str_get");
    int ret = m_redis_server->strget2(key,value,m_update_key_expire_time);

    if(ret == -1)
    {
        LOG4CPLUS_WARN(logger, "get " << dump() << " key:" << key << " failed, close connection with it");
        m_redis_server->close();
        return false;
    }
    LOG4CPLUS_DEBUG(logger, "str_get key : " << key << " value " << value << "length" <<  value.length());
    return ret;

}
bool SDRedisStringHandler::str_set(const string& key,const string& value)
{
    CHECK_CONN("StringHandler","update");

    LOG4CPLUS_DEBUG(logger, "set " << dump() << " key:" << key << "value:" << value);

    int ret = m_redis_server->strset2(key, value, m_set_key_expire_time);

    // 关闭连接
    if(ret == -1)
    {
        LOG4CPLUS_WARN(logger, "update " << dump() << " key:" << key << " failed, close connection with it");
        m_redis_server->close();
    }
    return ret == 1 ? true: false;
}

bool SDRedisStringHandler::str_remove(const std::string& key)
{
    CHECK_CONN("StringHandler","remove");

    LOG4CPLUS_DEBUG(logger, "remove" << dump() << " key:" << key);

    int ret = m_redis_server->remove(key);

    // 关闭连接
    if(ret == -1)
    {
        LOG4CPLUS_WARN(logger, "remove " << dump() << " key:" << key << " failed, close connection with it");
        m_redis_server->close();
    }

    return ret == 1 ? true: false;
}

void SDRedisStringHandler::reset_status(void)
{
	return;
}

std::string SDRedisStringHandler::get_statistic(int reset)
{
	stringstream output;
 
	if(reset)
	{
		reset_status();
	}
	
	return output.str();
}

