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
#include "utils/log.h"
#include "utils/pathext.h"
#include "frontend/parser.h"
#include "frontend/astvisitor.h"
#include "frontend/xmlwritevisit.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>

#include "utils/modulepath.h"

const int	CHECK_SUC = 0;
const int	CHECK_FAIL = 1;

int main(int argc,const char * argv[])
{
	//initionlize config.
	if(!ioc::utils::Option::instance().initFromArgs(argc,argv))
	{
		std::cout << CHECK_FAIL;
		return CHECK_FAIL;
	}

	std::string source("system.source");
	if(ioc::utils::Option::instance().is_existed(source))
	{
		std::vector<std::string> srcset = ioc::utils::Option::instance().get<std::vector<std::string> >(source);
		std::vector<std::string>::iterator it = srcset.begin();
		bool checkFailed = false;
		while(it != srcset.end())
		{
			boost::shared_ptr<ioc::frontend::Parser> pParser;
			pParser.reset(new ioc::frontend::Parser());
			if(pParser->parser(*it))
			{
				std::string	key("system.output");
				boost::filesystem::path	result;
				if(ioc::utils::Option::instance().is_existed(key))
				{//system.output is exist.use it.
					boost::filesystem::path	p(ioc::utils::Option::instance().get<std::string>(key));
					if(p.is_absolute())
					{
						result = p;
					}else{
						result = ioc::utils::ModulePath::instance().initPath();
						result /= p;
					}
				}else{//use current input file.
					boost::filesystem::path	p(*it);
					if(p.is_absolute())
					{
						result = p;
					}else{
						result = ioc::utils::ModulePath::instance().initPath();
						result /= p;
					}
					result.replace_extension("xml");
				}
				ioc::AstNode* pNode = ioc::XmlWriteVisit::ReadFrom(result.generic_string());
				if(!pNode || !pNode->logicEqual(pParser->getRootAst()) )
				{
					checkFailed = true;
					break;
				}
			}
			it++;
		}
		
		if(!checkFailed)
		{
			std::cout << CHECK_SUC;
			return CHECK_SUC;
		}
	}
	std::cout << CHECK_FAIL;
	return CHECK_FAIL;
}
