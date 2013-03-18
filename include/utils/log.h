////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  //
//                                                                        //
//  This file is part of IOC.                                             //
//                                                                        //
//  IOC is free software; you can redistribute it and//or modify it under //
//  the terms of the IOC Public License(LGPL License) as published by     //
//  masol.lee.                                                            //
//                                                                        //
//  IOC is distributed in the hope that it will be useful,but WITHOUT     //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                  //
//                                                                        //
//  You should have received a copy of the IOC License along with this    //
//  program.  If not, see <http://www.masols.com/licenses>.               //
//                                                                        //
//  IOC website: http://www.masols.com                                    //
////////////////////////////////////////////////////////////////////////////

/**
 * @file        log.h
 * @brief       提供ioc的日志机制
 **/


#ifndef  IOC_UTILS_LOG_H
#define  IOC_UTILS_LOG_H

#include "utils/singleton.h"
#include "utils/c_log.h"
#include <boost/thread/thread.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/init/from_stream.hpp>
#include <boost/log/utility/init/to_file.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/regex.hpp> 
//本头文件也被configuration所使用。
#include <boost/thread/mutex.hpp>
//#endif
#include <map>


namespace ioc { namespace utils
{
    /** @defgroup logger 日志子系统
	 * @ingroup utils
     * @brief   ioc的日志基于boost::log提供了全局的线程安全日志子系统，使用者不需要了解太多的日志知识就可以使用。
	 * @details  基于boost::log的一个封装，类似于boost::log中的trivial logging.如果需要更多的日志功能，需要深入了解boost::log库，不在本文的处理范围之内。
     * 缺省未经配置的日志记录会将大于等于WARN的日志输出到控制台。
     * 使用举例:
	 * @code
     * IOC_LOG() << "this is a log with pre-defined severity";
     * IOC_LOG_SEV(ERROR) << "message been logged to error severity";
   * @endcode
     * 这等效于:
	 * @code
     * BOOST_LOG(ioc::Log::instance().blog()) << "this is a log with pre-defined severity";
     * BOOST_LOG_SEV(ioc::Log::instance().blog(),ioc::Log::ERROR) << "message been logged to error severity";
   * @endcode
	 * @see LogTest
	 * @see https://dev.spolo.org/trac/wiki/SPLib_Logger
	 * @{
     **/
    class Log : public Singleton<Log>
    {
        private:
            friend class Singleton<Log>;
            typedef Singleton<Log>   inherited;
            typedef boost::shared_ptr<boost::log::sinks::aux::synchronous_frontend< char > > sinktype;
            typedef boost::shared_ptr<boost::BOOST_LOG_NAMESPACE::sinks::synchronous_sink<boost::BOOST_LOG_NAMESPACE::sinks::basic_text_file_backend<char> > > filesinktype;

        public:
            //{{{结构和类型定义 begin
            enum SEVERITY_LEVEL
            {
                NORM = IOC_LOG_SEV_NORM,
                DEBUG = IOC_LOG_SEV_DEBUG,
                INFO = IOC_LOG_SEV_INFO,
                NOTICE = IOC_LOG_SEV_NOTICE,
                WARN = IOC_LOG_SEV_WARN,
                _ERROR = IOC_LOG_SEV_ERROR, // 在windows下ERROR是关键字
                EMERG = IOC_LOG_SEV_EMERG,
            };

	private:
			//TODO we need support thread under IOC__OS_SOLARIS. see [ticket:63#comment:1]
            typedef boost::log::sources::severity_logger_mt<SEVERITY_LEVEL> blog_source_type;
		

            /**缺省的日志最低级别 */
            static const SEVERITY_LEVEL DEFAULT_LEVEL = WARN;
            /**缺省的日志备份大小 */
            static const int DEFAULT_ROT_SIZE = 1024*1024*1024;


            /**
             * @brief           备份日志文件的事件
             * @details         如果mday为0，则表示每天备份一次，
             *                  如果mday为1到31，则表示每个月备份一次
             *                  在配置信息中表现为"day hh:mm::sec"
             **/
            struct RTime
            {
                unsigned short m_day;       /*每个月的天 */
                unsigned char m_hour;       /*时 */
                unsigned char m_minute;     /*分 */
                unsigned char m_second;     /*秒 */
                RTime(unsigned short day = 0, unsigned char hour = 0,
                        unsigned char minute = 0, unsigned char sec=0):
                    m_day(day), m_hour(hour), m_minute(minute), m_second(sec)
                {}

            };
            /**
             * @brief           TextFile Sink's info
             **/
            struct TextSinkInfo
            {
                std::string     m_filename;         /**<文件名 */
                size_t          m_rotation_size;    /**<备份大小 */
                SEVERITY_LEVEL  m_level;            /**<最底的日志级别 */
                bool            m_auto_flush;       /**<是否auto flush */
                RTime           m_rotation_time;    /**<备份时间 */
				bool			m_rotation_time_setted;  /** 是否设置了回滚时间？ */
                TextSinkInfo(std::string file, size_t r_size=DEFAULT_ROT_SIZE,
                        SEVERITY_LEVEL level=DEFAULT_LEVEL, bool auto_flush=true):
                        m_filename(file), m_rotation_size(r_size),
                        m_level(level), m_auto_flush(auto_flush),
                        m_rotation_time(0, 0, 0, 0),m_rotation_time_setted(false){}
            };


            /**
             * @brief           Console Sink's Info
             **/
            struct ConsSinkInfo
            {
                std::ostream & m_strm;              /**<console's stream */
                SEVERITY_LEVEL  m_level;            /**<最底的日志级别 */
                ConsSinkInfo(std::ostream &strm, 
                    SEVERITY_LEVEL level=DEFAULT_LEVEL):m_strm(strm),
                    m_level(level){}
            };

            //}}}结构和类型定义 end

        public:

            //{{{对外接口 begin
            operator blog_source_type(void) const {
                return m_lgs;
            }
            operator blog_source_type&(void){
                return m_lgs;
            }
            blog_source_type& blog(void){
                return m_lgs;
            }

            
            /**
             * @brief           配置日志信息
             * @param stm       [IN], 输入流，通过读取流信息对日志进行配置
             **/
            void configuration(std::istream &stm);
            
            ~Log(){}
            //}}}对外接口 end



        protected:

            /**
             * @brief           把输出列表置空
             **/
            void resetToEmpty(void)
            {
                boost::shared_ptr< boost::log::core > core = boost::log::core::get();
                std::for_each(m_sinklist.begin(),m_sinklist.end(),boost::bind(&boost::log::core::remove_sink,core.get(),_1 ) );
                core->reset_filter();
                m_sinklist.clear();
            }


            /**
             * @brief           填充一个TextSinkInfo信息
             * @param pmap      [IN], TextSinkInfo属性map
             * @param tsi       [OUT], TextSinkInfo信息
             **/
            void FillTsi(std::map<std::string, std::string> &pmap, TextSinkInfo & tsi);

            /**
             *  @brief          添加一个TextFile类型的Sink
             *  @param tsi      [IN], TextFile Sink所需要的信息
             **/
            void AddFileSink(TextSinkInfo & tsi);

            /**
             *  @brief          添加一个Console类型的Sink
             *  @param ssi      [IN], Console Sink所需要的信息
             **/
            void AddConsSink(ConsSinkInfo & ssi);

            /**
             * @brief           把字符串转化为一个SEVERITY_LEVEL
             * @param level     [IN], 字符串的级别
             **/
            boost::optional<SEVERITY_LEVEL> string_2_level(const std::string & level);

            //{{{配置信息中的属性名
            const char * Destination()  {return "destination";}
            const char * FileName()     {return "filename";}
            const char * AutoFlush()    {return "autoflush";}
            const char * RotationSize() {return "rotationsize";}
            const char * RotationTime() {return "rotationtime";}
            const char * MinLevel()     {return "minlevel";}
            const char * ConsStream()   {return "stream";}
            //}}}
            
            //{{{配置信息中一些预定义好的属性值

            //Destination 属性可能的值
            const char * TextFile()     {return "textfile";}
            const char * Console()      {return "console";}

            //ConsStream可能的值
            const char * StdOut()       {return "stdout";}
            const char * StdErr()       {return "stderr";}
            //}}}

        private:
            //{{{私有成员 begin
            blog_source_type        m_lgs;          /**<日志源 */
            std::list<sinktype>     m_sinklist;
			
            mutable boost::shared_mutex m_mutex;

            Log() : m_lgs(WARN)
            {
                ///缺省初始化为NO LOG.而不是WARN级别下的输出到控制台的日志子系统。
                //sinktype sc = boost::log::init_log_to_console();
                //m_sinklist.push_back(sc);
            }/* constructor */

            //}}}私有成员 end
    }; /* -----  end of class Log  ----- */


/** @brief helper macro to use ioc log.
 * 
**/
#define IOC_LOG()             BOOST_LOG(ioc::utils::Log::instance().blog())
#define IOC_LOG_SEV(sev)      BOOST_LOG_SEV(ioc::utils::Log::instance().blog(),ioc::utils::Log::sev)


/**
 *@brief            把Log::SEVERITY_LEVEL输出成字符串
 **/
template< typename CharT, typename TraitsT >
std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, Log::SEVERITY_LEVEL lvl)
{
    const char* const str[] =
    {
        "NORM",
        "DEBUG",
        "INFO",
        "NOTICE",
        "WARN",
        "ERROR",
        "EMERG"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

/**@}*/ //end group of logger

}/*-----end of namespace utils--------------*/
}/*------end of namespace ioc-------------*/

#endif   /* ----- #ifndef IOC_UTILS_LOG_H  ----- */
