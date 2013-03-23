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
#include "frontend/parser.h"
#include "frontend/astvisitor.h"
#include "frontend/xmlwritevisit.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>

#include "utils/modulepath.h"

class visitorAAA : public ioc::AstVisitor{
		int indent;
	public:
		visitorAAA(){
			indent = 0;
		}
		virtual void beginTraversal(ioc::AstNode * node) {
			for(int i = 0 ; i < indent * 4; i++)
			{
				std::cout << ' ';
			}
			std::cout << node->printable_type_name() << ":";
			std::cout << node->childrenCount() << std::endl;
			//ioc::Assignment *pAssignNode = node->AsAssignment();
			//if(pAssignNode)
			//{
			//	ioc::AstNode* pLeft = pAssignNode->left();
			//	ioc::AstNode* pRight = pAssignNode->right();
			//}
			indent++;
		}
		virtual void endTraversal(ioc::AstNode * node) {	indent--;}
	};


int main(int argc,const char * argv[])
{
	//initionlize config.
	if(!ioc::utils::Option::instance().initFromArgs(argc,argv))
		return 0;

	std::string source("system.source");
	if(ioc::utils::Option::instance().is_existed(source))
	{
		std::vector<std::string> srcset = ioc::utils::Option::instance().get<std::vector<std::string> >(source);
		std::vector<std::string>::iterator it = srcset.begin();
		while(it != srcset.end())
		{
			std::cout << "Compiling " << *it << std::endl;

			boost::shared_ptr<ioc::frontend::Parser> pParser;
			pParser.reset(new ioc::frontend::Parser());
			if(pParser->parser(*it))
			{
                if(ioc::utils::Option::instance().is_existed("system.genxml"))
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
					ioc::XmlWriteVisit  visit;
					visit.WriteTo(pParser->getRootAst(),result.generic_string());
				}
				visitorAAA	v;
				v.apply(pParser->getRootAst());
			}

			it++;
		}
	}
	IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
	printf("hello,finish");
	return 0;
}
