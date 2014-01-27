/********************************************************************
	created:		2005/08/16
	created:		16:8:2005   21:07
	filename: 		d:\work\newcvs\chub\impl\src\common\sdlogger.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdlogger
	file ext:		h
	author:			lifeng
	description:	Switch of logger
*********************************************************************/

#ifndef SANDAI_C_LOGGER_H_200508162107
#define SANDAI_C_LOGGER_H_200508162107

#ifdef LOGGER
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
using namespace log4cplus;
#define DECL_LOGGER(logger) static log4cplus::Logger logger;
#define IMPL_LOGGER(classname, logger) log4cplus::Logger classname::logger = log4cplus::Logger::getInstance(#classname);

#ifndef _DEBUG

#ifdef LOG4CPLUS_DEBUG
#undef LOG4CPLUS_DEBUG
#define LOG4CPLUS_DEBUG(a,b)
#endif

#ifdef LOG4CPLUS_INFO
#undef LOG4CPLUS_INFO
#define LOG4CPLUS_INFO(a,b)
#endif

#endif // #ifndef _DEBUG

#else  // #ifdef LOGGER
#define _LOG4CPLUS_LOGGING_MACROS_HEADER_
#define LOG4CPLUS_DEBUG(a,b)
#define LOG4CPLUS_INFO(a,b)
#define LOG4CPLUS_WARN(a,b)
#define LOG4CPLUS_ERROR(a,b)
#define LOG4CPLUS_FATAL(a,b)

#define DECL_LOGGER(logger)
#define IMPL_LOGGER(classname, logger)

#endif

#endif

