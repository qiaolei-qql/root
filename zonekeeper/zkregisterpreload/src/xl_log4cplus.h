/*******************************************************************************
* ��Ȩ���� (C)2009 ������Ѹ�����缼�����޹�˾��
* ϵͳ����  : Ѹ�׹�����
* �ļ�����  : xl_log4cplus.h
* ����ժҪ  : Ҫʹ�� xl_log4cplus ��Ҫ������Ψһͷ�ļ�
* ��ǰ�汾  : 1.0
* ��    ��  : ������
* �������  : 2009��4��16��
* �޸ļ�¼  : 
* ��    ��      ��    ��        �޸���      �޸�ժҪ
*******************************************************************************/

/********************************* ʹ��˵�� ***********************************/

/**************************** ��������ѡ���ͷ�ļ� ****************************/
#ifndef __XL_LOG4CPLUS_H_77D80A29_3746_4E1D_AD42_855D11851904__
#define __XL_LOG4CPLUS_H_77D80A29_3746_4E1D_AD42_855D11851904__

#ifdef RECORD_LOG
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#endif

XL_NAMESPACE_BEGIN(lnx)

/****************************** XL_LOG4CPLUS ��׼�� ***************************/

#ifndef RECORD_LOG

#define LOG_INIT()
#define LOG_INIT_EX(cfgfile)

// log in class
#define LOG_CLS_DEC()
#define LOG_CLS_DEC_EX(loggername)
#define LOG_METHOD()
#define LOG_TRACE(msg)
#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)
#define LOG_FATAL(msg)

// log in global
#define LOGGER_DEC(logger)
#define LOGGER_IMP(logger, loggername)
#define LOGGER_IMP_EX(logger, loggername)
#define LOGGER_METHOD(logger)
#define LOGGER_TRACE(logger,msg)
#define LOGGER_DEBUG(logger,msg)
#define LOGGER_INFO(logger,msg)
#define LOGGER_WARN(logger,msg)
#define LOGGER_ERROR(logger,msg)
#define LOGGER_FATAL(logger,msg)

#else // !defined(RECORD_LOG)

#define LOG_INIT()                              log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("xl_log4cplus.cfg"))
#define LOG_INIT_EX(cfgfile)                    log4cplus::PropertyConfigurator::doConfigure(cfgfile)

// log in global
#define LOGGER_DEC(logger)                        extern log4cplus::Logger logger
#define LOGGER_IMP(logger, loggername)            LOGGER_IMP_EX(logger, LOG4CPLUS_C_STR_TO_TSTRING(__FILE__))
#define LOGGER_IMP_EX(logger, loggername)         log4cplus::Logger logger = log4cplus::Logger::getInstance(loggername)
#define LOGGER_METHOD(logger)                     LOG4CPLUS_TRACE_METHOD(logger, LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__))
#define LOGGER_TRACE(logger,msg)                  LOG4CPLUS_TRACE(logger, '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)
#define LOGGER_DEBUG(logger,msg)                  LOG4CPLUS_DEBUG(logger, '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)
#define LOGGER_INFO(logger,msg)                   LOG4CPLUS_INFO(logger,  '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)
#define LOGGER_WARN(logger,msg)                   LOG4CPLUS_WARN(logger,  '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)
#define LOGGER_ERROR(logger,msg)                  LOG4CPLUS_ERROR(logger, '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)
#define LOGGER_FATAL(logger,msg)                  LOG4CPLUS_FATAL(logger, '[' << LOG4CPLUS_C_STR_TO_TSTRING(__FILE__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << LOG4CPLUS_C_STR_TO_TSTRING(__PRETTY_FUNCTION__) << LOG4CPLUS_C_STR_TO_TSTRING(" - ") << __LINE__ << ']' << msg)

// log in class
#define LOG_CLS_DEC()                           LOG_CLS_DEC_EX(LOG4CPLUS_C_STR_TO_TSTRING(__FILE__))
#define LOG_CLS_DEC_EX(loggername)              static log4cplus::Logger & getLogger() { static log4cplus::Logger s_logger = log4cplus::Logger::getInstance(loggername); return s_logger; }

#define LOG_METHOD()                            LOGGER_METHOD(getLogger())
#define LOG_TRACE(msg)                          LOGGER_TRACE(getLogger(),msg)
#define LOG_DEBUG(msg)                          LOGGER_DEBUG(getLogger(),msg)
#define LOG_INFO(msg)                           LOGGER_INFO(getLogger(),msg)
#define LOG_WARN(msg)                           LOGGER_WARN(getLogger(),msg)
#define LOG_ERROR(msg)                          LOGGER_ERROR(getLogger(),msg)
#define LOG_FATAL(msg)                          LOGGER_FATAL(getLogger(),msg)

#endif // RECORD_LOG

/********************************** �������ؿ�� **********************************/

#ifndef RECORD_LOG
#define _LOG4CPLUS_LOGGING_MACROS_HEADER_
#define LOG4CPLUS_TRACE(a,b)
#define LOG4CPLUS_DEBUG(a,b)
#define LOG4CPLUS_INFO(a,b)
#define LOG4CPLUS_WARN(a,b)
#define LOG4CPLUS_ERROR(a,b)
#define LOG4CPLUS_FATAL(a,b)

#define LOG4CPLUS_INIT(param)
#define LOG4CPLUS_DECLARE(logger)
#define LOG4CPLUS_IMPLEMENT(logger, name)
#define LOG4CPLUS_CLASS_DECLARE(logger)
#define LOG4CPLUS_CLASS_IMPLEMENT(classname, logger, name)

#define LOG4CPLUS_THIS_TRACE(a,b)
#define LOG4CPLUS_THIS_DEBUG(a,b)
#define LOG4CPLUS_THIS_INFO(a,b)
#define LOG4CPLUS_THIS_WARN(a,b)
#define LOG4CPLUS_THIS_ERROR(a,b)
#define LOG4CPLUS_THIS_FATAL(a,b)

#else // !defined(RECORD_LOG)

#define LOG4CPLUS_INIT(param)	                            log4cplus::PropertyConfigurator::doConfigure(param)
#define LOG4CPLUS_DECLARE(logger)			                extern log4cplus::Logger logger
#define LOG4CPLUS_IMPLEMENT(logger, name)	                log4cplus::Logger logger = log4cplus::Logger::getInstance(name)
#define LOG4CPLUS_CLASS_DECLARE(logger)						LOG_CLS_DEC()
#define LOG4CPLUS_CLASS_IMPLEMENT(classname, logger, name)

#define LOG4CPLUS_THIS_TRACE(a,b)	LOG_TRACE(b)
#define LOG4CPLUS_THIS_DEBUG(a,b)	LOG_DEBUG(b)
#define LOG4CPLUS_THIS_INFO(a,b)	LOG_INFO(b)
#define LOG4CPLUS_THIS_WARN(a,b)	LOG_WARN(b)
#define LOG4CPLUS_THIS_ERROR(a,b)	LOG_ERROR(b)
#define LOG4CPLUS_THIS_FATAL(a,b)	LOG_FATAL(b)

#endif // RECORD_LOG

/********************************** ���ݿ����� TSLog **********************************/

#ifndef RECORD_LOG

#define TSTRACE
#define TSDEBUG
#define TSINFO
#define TSWARN
#define TSERROR
#define TSFATAL

#else // !defined(RECORD_LOG)

#define TSTRACE     LOG_TRACE
#define TSDEBUG     LOG_DEBUG
#define TSINFO      LOG_INFO
#define TSWARN      LOG_WARN
#define TSERROR     LOG_ERROR
#define TSFATAL     LOG_FATAL

#endif // RECORD_LOG

XL_NAMESPACE_END(lnx)

#endif // end of __XL_LOG4CPLUS_H_77D80A29_3746_4E1D_AD42_855D11851904__
