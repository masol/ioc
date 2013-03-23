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
#include "utils/modulepath.h"
#include <boost/program_options.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include "utils/log.h"


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
		("help,h", "Display this information.")
		("version,v", "Display  version informationl.")
		("genxml,x", "Generate the ast(xml) file.")
		("config,c", boost::program_options::value<std::string>(),"Specify config file.")
		("source,s", boost::program_options::value<std::string>(),"Specify source file.")
		("output,o", boost::program_options::value<std::string>(),"Place the output into <file>.")
		;

//all property begin with log treat with log config,pass it to log.
//	generic.add_options()
//注释size的原因是缺省分析器不支持乘法与后缀。
//		("log.size", boost::program_options::value<int>(),"specify log file max size,0 to disable rollover.")
//		("log.index", boost::program_options::value<int>(),"specify rollover log file count.")
//		("log.out2std", boost::program_options::value<bool>(),"standard log output to standard out simultaneously.")
//		("log.loglevel", boost::program_options::value<std::string>(),"standard log level.can be any of below:\n  EMERG ALERT CRIT ERROR WARN INFO DEBUG NOTSET\n  or a number.")
//		("log.accessfile.size", boost::program_options::value<int>(),"specify accesslog file max size,0 to disable rollover.")
//		("log.accessfile.index", boost::program_options::value<int>(),"specify rollover access log file count.")
//		("log.accessfile.out2std", boost::program_options::value<bool>(),"access log output to standard out simultaneously.")
//		("log.accessfile.loglevel", boost::program_options::value<std::string>(),"access log level.can be any of below:\n  EMERG ALERT CRIT ERROR WARN INFO DEBUG NOTSET\n  or a number.")
//		;
}

struct  parser_context{
    boost::program_options::options_description     &cmdline_options;
    std::vector<std::string>                      sources;
    std::stringstream                               log_cfg;
    bool                                            genxml;
    std::string                                     outfile;
    parser_context(boost::program_options::options_description &clo) : cmdline_options(clo),genxml(false)
    {
    }
};

static inline bool loadConfigFile(const std::string &cfgFile,parser_context &ctx);
static	bool	parser_Option(boost::program_options::parsed_options &option,parser_context &ctx)
{
	bool bNeedQuit = false;
	std::vector< boost::program_options::option>::iterator	it = option.options.begin();
	std::string	 log_tag("log.");

	std::vector<std::string>	configFile;
	while(it != option.options.end())
	{
		if(it->unregistered)
		{//发现一个未注册的配置选项。
			//TODO: more flexible mechanism to extend config.
			if(it->string_key.compare(0, log_tag.length(), log_tag) == 0)
			{
				ctx.log_cfg << it->string_key.substr(log_tag.length()) << "=";
				std::vector< std::string>::iterator	value_it = it->value.begin();
				while(value_it != it->value.end() )
				{
					ctx.log_cfg << *value_it;
					value_it++;
				}
				ctx.log_cfg << std::endl;
			}else{
				outCopyRights();
				std::cout << "invalid parameter " << it->string_key << std::endl;
				outUsage();
				std::cout << ctx.cmdline_options << std::endl;
				bNeedQuit = true;
				break;
			}
		}else if(it->string_key == "config")
		{
			if(it->value.size() == 0)
			{
				outCopyRights();
				std::cout << "config need a config file as parameter" << std::endl;
				outUsage();
				bNeedQuit = true;
				break;
			}
			configFile = it->value;
			std::vector<std::string>::iterator cfgit = configFile.begin();
			bool	bcfgFileOK = true;
			while(cfgit != configFile.end())
			{
				if(!boost::filesystem::exists(*cfgit))
				{
					outCopyRights();
					std::cout << "specified config file " << *cfgit << " doesn't exist" << std::endl;
					bcfgFileOK = false;
					bNeedQuit = true;
					break;
				}
				cfgit++;
			}
			if(!bcfgFileOK)
			{
				break;
			}
		}else if(it->string_key == "help")
		{
			outCopyRights();
			outUsage();
			std::cout << ctx.cmdline_options << std::endl;
			bNeedQuit = true;
			break;
		}else if(it->string_key == "version")
		{
			outCopyRights();
			std::cout << "IOC Version "<< IOC_VERSION_MAJOR << '.' << IOC_VERSION_MINOR << std::endl;
			bNeedQuit = true;
			break;
		}else if(it->string_key == "source")
		{
			std::vector<std::string>::iterator value_it = it->value.begin();
			while(value_it != it->value.end())
			{
				if(std::find(ctx.sources.begin(),ctx.sources.end(),*value_it) == ctx.sources.end())
				{
					ctx.sources.push_back(*value_it);
				}
				value_it++;
			}
		}else if(it->string_key == "genxml")
		{
            ctx.genxml = true;
		}else if(it->string_key == "output")
		{
		    if(it->value.size() > 0)
                ctx.outfile = *(it->value.begin());
		}
		it++;
	}

	//开始分析和保存配置文件的内容。
	if(!bNeedQuit && configFile.size())
	{
		std::vector<std::string>::iterator cfgit = configFile.begin();
		while(cfgit != configFile.end())
		{
			if(loadConfigFile(*cfgit,ctx))
			{
				bNeedQuit = true;
				break;
			}
			cfgit++;
		}
	}

	return bNeedQuit;
}

static inline bool loadConfigFile(const std::string &cfgFile,parser_context &ctx)
{
	std::ifstream	input(cfgFile.c_str(),std::ios_base::in);
	BOOST_SCOPE_EXIT( (&input))
	{
		input.close();
	}BOOST_SCOPE_EXIT_END

	bool	bNeedQuit = false;
	if(input.is_open())
	{
		std::cout << "loading config file " << cfgFile << " ...";
		boost::program_options::parsed_options po = boost::program_options::parse_config_file(input,ctx.cmdline_options,true);
		bNeedQuit = parser_Option(po,ctx);
		std::cout << "done!" << std::endl;
	}else{
		outCopyRights();
		std::cout << "can not open config file " << cfgFile << std::endl;
		outUsage();
		bNeedQuit = true;
	}
	return bNeedQuit;
}

bool
Option::initFromArgs(int argc,const char* argv[])
{
	boost::program_options::options_description		cmdline;
	boost::program_options::options_description		generic;

	initOption_descritpion(cmdline,generic);

	bool	bNeedQuit = false;
	boost::program_options::options_description cmdline_options;
	cmdline_options.add(cmdline).add(generic);

	boost::program_options::positional_options_description p;
	p.add("source", -1);
	boost::program_options::parsed_options po = boost::program_options::command_line_parser(argc,argv).options(cmdline_options).positional(p).allow_unregistered().run();

	parser_context      ctx(cmdline_options);
	bNeedQuit = parser_Option(po,ctx);
	if(!bNeedQuit)
	{//system runing, set the source and config log system.
		if(ctx.sources.size())
		{
			this->put("system.source",ctx.sources);
		}
		if(ctx.genxml)
        {
            this->put("system.genxml",true);
        }
        if(ctx.outfile.length())
        {
            this->put("system.output",ctx.outfile);
        }
		//@todo : po only from command line,process it in config file.
		if(ctx.log_cfg.tellp() > 0)
		{
			std::cout << "log_cfg : " << ctx.log_cfg.str() << std::endl;
			Log::instance().configuration(ctx.log_cfg);
		}
		ioc::utils::ModulePath::instance().initialize(argv[0]);
	}

	return !bNeedQuit;
}

}//utils
}//ioc
