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
#include <stdio.h>


int main(int argc,const char * argv[])
{
	//initionlize config.
	if(!ioc::utils::Option::instance().initFromArgs(argc,argv))
		return 0;

	std::string source("source");
	if(ioc::utils::Option::instance().is_existed(source))
	{
		std::vector<std::string> srcset = ioc::utils::Option::instance().get<std::vector<std::string>>(source);
		std::vector<std::string>::iterator it = srcset.begin();
		while(it != srcset.end())
		{
			std::cout << "Compiling " << *it << std::endl;
			it++;
		}
	}
	IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
	printf("hello,finish");
	return 0;
}