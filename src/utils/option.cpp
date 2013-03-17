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

#include "config.h"
#include "utils/option.h"

namespace ioc { namespace utils {

static inline void		outCopyRights(void){
	std::cout << "(C) copyright 2013 masol.lee." << std::endl;
	std::cout << "This is free software; see the source for copying conditions.  There is NO " << std::endl;
	std::cout << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl;
	std::cout << "Written by masol.lee<masol.li@gmail.com>. " << std::endl;
}

static inline void		outUsage(void){
	std::cout << "Usage: ioc [options] [source-file]" << std::endl;
}

static inline void		initOption_descritpion(boost::program_options::options_description &cmdline,boost::program_options::options_description &generic)
{
	cmdline.add_options()
		("help,h", "print help message")
		("version,v", "print version information")
		("config,c", boost::program_options::value<std::string>(),"specify config file")
		;

	generic.add_options()
//注释size的原因是缺省分析器不支持乘法与后缀。
//		("log.size", boost::program_options::value<int>(),"specify log file max size,0 to disable rollover.")
		("log.index", boost::program_options::value<int>(),"specify rollover log file count.")
		("log.out2std", boost::program_options::value<bool>(),"standard log output to standard out simultaneously.")
		("log.loglevel", boost::program_options::value<std::string>(),"standard log level.can be any of below:\n  EMERG ALERT CRIT ERROR WARN INFO DEBUG NOTSET\n  or a number.")
//		("log.accessfile.size", boost::program_options::value<int>(),"specify accesslog file max size,0 to disable rollover.")
		("log.accessfile.index", boost::program_options::value<int>(),"specify rollover access log file count.")
		("log.accessfile.out2std", boost::program_options::value<bool>(),"access log output to standard out simultaneously.")
		("log.accessfile.loglevel", boost::program_options::value<std::string>(),"access log level.can be any of below:\n  EMERG ALERT CRIT ERROR WARN INFO DEBUG NOTSET\n  or a number.")
		;
}

bool
Option::init(int argc,const char* argv[])
{
	boost::program_options::options_description		cmdline;
	boost::program_options::options_description		generic;
	boost::program_options::variables_map 			varmap;
	boost::unordered_map<std::string,std::string>	unRegVarMap;
	
	initOption_descritpion(cmdline,generic);

	return true;
}


}//utils
}//ioc
