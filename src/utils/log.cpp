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
 * @file        log.cpp
 * @brief       ioc::Log的实现
 **/
#include "config.h"
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "utils/log.h"

namespace logging = boost::log;
namespace fmt = boost::log::formatters;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

namespace ioc{ namespace utils
{

    /**
     * @brief               配置日志信息
     * @param stm           [IN], 输入流，通过读取流信息对日志进行配置
     */
    void Log::configuration(std::istream &stm)
    {
        //TODO: write our configuration front-end,use init_from_stream may cause many linkage requirement.
        //boost::log::init_from_stream(stm);

        boost::unique_lock<boost::shared_mutex> l(m_mutex);

        resetToEmpty();
        std::string  line; //current line.
    
        //首先收集信息，按照Sink的名称建立Hash-map。
        //如果hash-map有效，则清空当前Sink配置。
        //然后根据这一Hash-map创建和配置Sink。
        typedef std::map<std::string, std::map<std::string,std::string> > type_props;
        type_props propmap;
        while(std::getline(stm,line))
        {
            //当前行以#开头，是一个注释行。
            if(line[0] == '#')
                continue;
            //每行的模式，用于提取参数。
            //FIX: .未转义。
            boost::regex Sink("\\s*[S|s]ink\\s*\\.\\s*(\\w+)\\s*\\.\\s*(\\w+)\\s*=\\s*(.+)");
            boost::cmatch result;
            if(boost::regex_match(line.c_str(),result,Sink))
            {
                const std::string &tmp = result[1].str();
                type_props::iterator it = propmap.find(result[1].str());
                if(it != propmap.end())
                {
                    std::map<std::string,std::string>::mapped_type &value = it->second[result[2].str()];
                    value = result[3].str();
                    boost::trim(value);
                }else{
                    std::map<std::string,std::string> tmpMap;
                    std::map<std::string,std::string>::mapped_type &value = tmpMap[result[2].str()];
                    value = result[3].str();
                    boost::trim(value);
                    propmap[result[1].str()] = tmpMap;
                }
            }
            else
            {
#ifdef _DEBUG
                std::cout<<"match failed"<<std::endl;
#endif
                continue;
            }
        }
        if(propmap.size())
        {
            type_props::iterator pit = propmap.begin();
            while(pit != propmap.end())
            {
                const std::string &type = pit->second[Destination()];
                if(type == TextFile())
                {
                    const std::string &filename = pit->second[FileName()];
                    if(!filename.empty())
                    {
                        TextSinkInfo tsi(filename);

                        /*填充TextSinkInfo结构 */
                        FillTsi((*pit).second, tsi);

                        /*添加一个TextFile Sink */
                        AddFileSink(tsi);
                    }
                }else if(type == Console())
                {
                    SEVERITY_LEVEL level = DEFAULT_LEVEL;
                    if(pit->second.count(MinLevel()))
                    {
                        std::string & strlevel = pit->second[MinLevel()];
                        boost::optional<SEVERITY_LEVEL> opt = string_2_level(strlevel);
                        if(opt.is_initialized())
                        {
                            level = *opt;
                        }
                    }

                    const std::string &stream = pit->second[ConsStream()];
                    if(stream == StdOut())
                    {
                        ConsSinkInfo ssi(std::cout, level);
                        AddConsSink(ssi);
                    }else
                    {
                        /* 缺省cerr */
                        ConsSinkInfo ssi(std::cerr, level);
                        AddConsSink(ssi);
                    }
                }
                pit++;
            }
        }
    } // end of configuration


    /**
     * @brief           填充一个TextSinkInfo信息
     * @param pmap      [IN], TextSinkInfo属性map
     * @param tsi       [OUT], TextSinkInfo信息
     **/
    void Log::FillTsi(std::map<std::string, std::string> &pmap, TextSinkInfo & tsi)
    {
        tsi.m_filename = pmap[FileName()];          /**<文件名 */
        if( pmap.count(RotationSize()) )
        {
            try{
                std::string& r_size = pmap[RotationSize()];
                tsi.m_rotation_size = boost::lexical_cast<size_t>(r_size);/**<备份大小 */
            }catch(const boost::bad_lexical_cast& e)
            {
				(void)e;
                tsi.m_rotation_size = DEFAULT_ROT_SIZE;
            }
        }
        if(pmap.count(AutoFlush()))
        {
            std::string& a_flush = pmap[AutoFlush()];
            if(a_flush == "False" || a_flush == "false" || a_flush == "FALSE")
            {
                tsi.m_auto_flush = false;
            }else
            {
                tsi.m_auto_flush = true;
            }
        }
        if(pmap.count(MinLevel()))
        {
            std::string & level = pmap[MinLevel()];
            /**最底的日志级别 */
            boost::optional<SEVERITY_LEVEL> opt = string_2_level(level);
            if(opt.is_initialized())
            {
                tsi.m_level = *opt;
            }
        }
        if(pmap.count(RotationTime()))
        {
            std::string & rtime = pmap[RotationTime()];

            boost::regex reg("((\\d{1,2})\\s+)?(\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
            boost::cmatch result;
            if(boost::regex_match(rtime.c_str(),result, reg))
            {
#ifdef _DEBUG
                std::cout<<"match successful"<<std::endl;
                std::cout<<"["<<result[1].str()<<"]"<<std::endl;
                std::cout<<"["<<result[2].str()<<"]"<<std::endl;
                std::cout<<"["<<result[3].str()<<"]"<<std::endl;
                std::cout<<"["<<result[4].str()<<"]"<<std::endl;
                std::cout<<"["<<result[5].str()<<"]"<<std::endl;
#endif

                try{
                    
                    if(!result[2].str().empty())
                    {
                        unsigned short day = boost::lexical_cast<unsigned short>(result[2].str());
                        if(day >= 1 && day <= 31)
                            tsi.m_rotation_time.m_day = day;

                    }


                    unsigned char hour = boost::lexical_cast<unsigned char>(result[3].str());
                    if(hour >= 0 && hour <= 23)
                        tsi.m_rotation_time.m_hour=hour;

                    unsigned char minute = boost::lexical_cast<unsigned char>(result[4].str());
                    if(minute >= 0 && hour <= 59)
                        tsi.m_rotation_time.m_minute= minute; 

                    unsigned char sec =boost::lexical_cast<unsigned char>(result[5].str());
                    if(sec >= 0 && sec <= 59)
                        tsi.m_rotation_time.m_second= sec;
                }catch(const boost::bad_lexical_cast& e)
                {
					(void)e;
                    /**类型转化失败，采用默认值 */
                }

#ifdef _DEBUG
                std::cout<<(long)tsi.m_rotation_time.m_day<<std::endl;
                std::cout<<(long)tsi.m_rotation_time.m_hour<<std::endl;
                std::cout<<(long)tsi.m_rotation_time.m_minute<<std::endl;
                std::cout<<(long)tsi.m_rotation_time.m_second<<std::endl;
#endif

            }
#ifdef _DEBUG
            else
            {
                std::cout<<"match failed"<<std::endl;
            }
#endif
        }

    }//end of FillTsi


    /**
     *  @brief          添加一个TextFile类型的Sink
     *  @param tsi      [IN], TextFile Sink所需要的信息
     **/
    void Log::AddFileSink(TextSinkInfo & tsi)
    {
        filesinktype sc;
				if(tsi.m_rotation_time_setted)
				{
						if(tsi.m_rotation_time_setted && tsi.m_rotation_time.m_day != 0)
						{
								sc= boost::log::init_log_to_file(
										tsi.m_filename,
										keywords::filter = flt::attr<SEVERITY_LEVEL>("Severity", std::nothrow) >= tsi.m_level,
										keywords::format = fmt::format("[%1%] <%2%> %3%")
												% fmt::date_time("TimeStamp", std::nothrow)
												% fmt::attr< SEVERITY_LEVEL >("Severity", std::nothrow)
												% fmt::message(),
										keywords::rotation_size = tsi.m_rotation_size,
										keywords::time_based_rotation = sinks::file::rotation_at_time_point
										(
												boost::gregorian::greg_day(tsi.m_rotation_time.m_day),
												tsi.m_rotation_time.m_hour,\
												tsi.m_rotation_time.m_minute, \
												tsi.m_rotation_time.m_second
										)
								);
						}else
						{
								sc= boost::log::init_log_to_file(
										tsi.m_filename,
										keywords::filter = flt::attr<SEVERITY_LEVEL>("Severity", std::nothrow) >= tsi.m_level,
										keywords::format = fmt::format("[%1%] <%2%> %3%")
												% fmt::date_time("TimeStamp", std::nothrow)
												% fmt::attr< SEVERITY_LEVEL >("Severity", std::nothrow)
												% fmt::message(),
										keywords::rotation_size = tsi.m_rotation_size,
										keywords::time_based_rotation = sinks::file::rotation_at_time_point
										(
												tsi.m_rotation_time.m_hour,\
												tsi.m_rotation_time.m_minute, \
												tsi.m_rotation_time.m_second
										)
								);
						}//if-else
				}else{
            sc= boost::log::init_log_to_file(
                tsi.m_filename,
                keywords::filter = flt::attr<SEVERITY_LEVEL>("Severity", std::nothrow) >= tsi.m_level,
                keywords::format = fmt::format("[%1%] <%2%> %3%")
                    % fmt::date_time("TimeStamp", std::nothrow)
                    % fmt::attr< SEVERITY_LEVEL >("Severity", std::nothrow)
                    % fmt::message(),
                keywords::rotation_size = tsi.m_rotation_size
            );
				}
        sc->locked_backend()->auto_flush(tsi.m_auto_flush);
        sc->locked_backend()->set_open_mode(std::ios_base::app|std::ios_base::out);

        logging::add_common_attributes();
        m_sinklist.push_back(sc);
    }

    /**
     *  @brief          添加一个Console类型的Sink
     *  @param ssi      [IN], Console Sink所需要的信息
     **/
    void Log::AddConsSink(ConsSinkInfo & ssi)
    {
        sinktype sc = boost::log::init_log_to_console(ssi.m_strm,
        keywords::filter = flt::attr<SEVERITY_LEVEL>("Severity", std::nothrow) >= ssi.m_level,
        keywords::format = fmt::format("[%1%] <%2%> %3%")
            % fmt::date_time("TimeStamp", std::nothrow)
            % fmt::attr< SEVERITY_LEVEL >("Severity", std::nothrow)
            % fmt::message()
            );
        m_sinklist.push_back(sc);
    }

    /**
     * @brief           把字符串转化为一个SEVERITY_LEVEL
     * @return          返回对应的SEVERITY_LEVEL, 失败返回SENULL
     **/
    boost::optional<Log::SEVERITY_LEVEL> Log::string_2_level(const std::string & level)
    {
		if(level == "NORM")
		{
				return boost::optional<SEVERITY_LEVEL>(NORM);
		}else if(level == "DEBUG")
		{
				return boost::optional<SEVERITY_LEVEL>(DEBUG);
		}else if(level == "INFO")
		{
				return boost::optional<SEVERITY_LEVEL>(INFO);
		}else if(level == "NOTICE")
		{
				return boost::optional<SEVERITY_LEVEL>(NOTICE);
		}else if(level == "WARN")
		{
				return boost::optional<SEVERITY_LEVEL>(WARN);
		}else if(level == "ERROR")
		{
				return boost::optional<SEVERITY_LEVEL>(_ERROR);
		}else if(level == "EMERG")
		{
				return boost::optional<SEVERITY_LEVEL>(EMERG);
		}
		return boost::optional<SEVERITY_LEVEL>();
    }

}//end of namespace utils
}// end of namespace ioc


extern "C"{

int ioc_isEanbled(unsigned int severity)
{
		return 1;
}

void ioc_log(unsigned int severity,const char* msg)
{
	switch(severity)
	{
	case IOC_LOG_SEV_NORM:
		IOC_LOG_SEV(NORM) << msg;
		break;
	case IOC_LOG_SEV_DEBUG:
		IOC_LOG_SEV(DEBUG) << msg;
		break;
	case IOC_LOG_SEV_INFO:
		IOC_LOG_SEV(INFO) << msg;
		break;
	case IOC_LOG_SEV_NOTICE:
		IOC_LOG_SEV(NOTICE) << msg;
		break;
	case IOC_LOG_SEV_WARN:
		IOC_LOG_SEV(WARN) << msg;
		break;
	case IOC_LOG_SEV_ERROR:
		IOC_LOG_SEV(_ERROR) << msg;
		break;
	default:
		IOC_LOG_SEV(EMERG) << msg;
	}
}

}