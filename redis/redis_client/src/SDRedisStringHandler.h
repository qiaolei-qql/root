#ifndef __SD_REDIS_STRING_HANDLER_H__
#define __SD_REDIS_STRING_HANDLER_H__

#include "SDRedisHandler.h"
#include <common/SDLogger.h>
//#include <common/SDPthreadLock.h>
#include <common/SDUtility.h>
#include <hiredis/hiredis.h>
#include <boost/shared_ptr.hpp>
#include "SDCondValue.h"
#include "SDRedisServer.h"
#include <string>
#include <vector>
#include <set>


class SDRedisStringHandler: public SDRedisHandler
{
    public:
        SDRedisStringHandler(SDRedisServer_ptr redis_server, uint32_t update_key_tos = 43200, uint32_t set_key_tos = 43200,uint32_t empty_key_tos=43200);

        ~SDRedisStringHandler();

/*        virtual SDRedisStringHandler* clone(SDRedisServer_ptr redis_server)
        {
            return new SDRedisStringHandler(redis_server, m_set_key_expire_time, m_update_key_expire_time,m_empty_key_expire_time);
        }
*/
        int str_get(const string& key,string& value);
        bool str_set(const string& key,const string& value);
        bool str_remove(const std::string& key);

        std::string get_statistic(int reset);

        void reset_status(void);

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

typedef boost::shared_ptr<SDRedisStringHandler> SDRedisStringHandler_ptr;

#endif	// __SD_REDIS_STRING_HANDLER_H__

