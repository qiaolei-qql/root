/*
 * filename      : base_define.h
 * descriptor    :  
 * author        : qiaolei
 * create time   : 2013-08-17 16:31
 * modify list   :
 * +----------------+---------------+---------------------------+
 * | date			| who			| modify summary			|
 * +----------------+---------------+---------------------------+
 */
#ifndef _BASE_DEFINE_H_
#define _BASE_DEFINE_H_
#include <stdio.h>
#include "global_var.h"

namespace hpf
{



#define ret_if_fail(condition) \
    if (!(condition)) \
    {\
        LOGGER_ERROR(g_logger, #condition" failed at " << __FILE__ << ":" << __LINE__);\
        return;\
    } 

#define ret_val_if_fail(condition, value)\
    if (!(condition)) \
    {\
        LOGGER_ERROR(g_logger, #condition" failed at " << __FILE__ << ":" << __LINE__);\
        return value;\
    }

}
#endif
