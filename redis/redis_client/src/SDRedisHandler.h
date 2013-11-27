#ifndef __SD_REDIS_HANDLER_H__
#define __SD_REDIS_HANDLER_H__

#include <common/SDLogger.h>
#include <boost/shared_ptr.hpp>
#include "SDRedisServer.h"
#include <string>
#include <vector>
using std::string;
using std::vector;


class SDRedisHandler
{
    public:
        static std::string HASH_HANDLER;
        static std::string SET_HANDLER;
        static std::string STRING_HANDLER;

    public:
        SDRedisHandler(SDRedisServer_ptr redis_server, uint32_t update_key_tos=43200, uint32_t set_key_tos=43200 ,uint32_t empty_key_tos=43200);

        virtual ~SDRedisHandler();

        bool init();

        void update_tos(uint32_t update_key_tos, uint32_t set_key_tos,uint32_t empty_key_tos)
        {
            m_update_key_expire_time = update_key_tos;
            m_set_key_expire_time = set_key_tos;
            m_empty_key_expire_time = empty_key_tos;
        };

        bool check_connection();
        std::string dump() {return m_redis_server->dump();}


    protected:

        SDRedisServer_ptr m_redis_server;

        uint32_t	m_update_key_expire_time;

        uint32_t	m_set_key_expire_time;

        uint32_t	m_empty_key_expire_time;

        uint32_t m_conn_timeout;		// connection timeout

    private:
        DECL_LOGGER(logger);

};

#define CHECK_CONN(handler_name, operation)\
    do {\
        if (!check_connection()) {\
            LOG4CPLUS_WARN(logger,(handler_name) << "-" << (operation) << " fail, cos conneciton to " << dump() << " error!");\
            return false;\
        }\
    } while (0)

typedef boost::shared_ptr<SDRedisHandler> SDRedisHandler_ptr;

#endif	// __SD_REDIS_HANDLER_H__


