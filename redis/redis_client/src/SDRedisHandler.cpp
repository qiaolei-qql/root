#include "SDRedisHandler.h"
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <btqueryserver/SDConfigurationSingleton.h>
#include <errno.h>
#include <assert.h>


IMPL_LOGGER(SDRedisHandler, logger);

std::string SDRedisHandler::HASH_HANDLER   = "HASH";
std::string SDRedisHandler::SET_HANDLER      = "SET";
std::string SDRedisHandler::STRING_HANDLER = "STRING";

SDRedisHandler::SDRedisHandler(SDRedisServer_ptr redis_server, uint32_t update_key_tos, uint32_t 
        set_key_tos,uint32_t empty_key_tos)
:m_redis_server(redis_server),
    m_update_key_expire_time(update_key_tos),
    m_set_key_expire_time(set_key_tos),
    m_empty_key_expire_time(empty_key_tos),
    m_conn_timeout(5)
{
    LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
    init();
}

SDRedisHandler::~SDRedisHandler()
{	
    LOG4CPLUS_DEBUG(logger, __FUNCTION__ << "(" << this << ")");
}

bool SDRedisHandler::init()
{
    LOG4CPLUS_DEBUG(logger, "SDRedisHandler start init");

    bool succ = true;

    if(!m_redis_server->is_connected())
    {
        succ =m_redis_server->connect(m_conn_timeout);
    }

    LOG4CPLUS_DEBUG(logger, "SDRedisHandler init succ!");

    return succ;	
}


bool SDRedisHandler::check_connection()
{
    if(!m_redis_server->is_connected())
    {
        LOG4CPLUS_DEBUG(logger, "redis server:" << dump() << " is not connected currently, try to connect at first.");
        bool succ = m_redis_server->connect(m_conn_timeout);
        if(!succ)
        {
            LOG4CPLUS_WARN(logger, "reconnection to:" << dump() << " failed.");
            return false;
        }
    }

    LOG4CPLUS_DEBUG(logger, "connection to:" << dump() << " is valid.");
    return true;
}
