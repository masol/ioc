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
#include "frontend/xmlparser.h"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <stdio.h>

#include "utils/modulepath.h"

#include "runtime/runtime.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/PrettyStackTrace.h"

class visitorAAA : public ioc::AstVisitor
{
	int indent;
public:
	visitorAAA(){
		indent = 0;
	}
	virtual bool beginTraversal(ioc::AstNode * node) {
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
		return true;
	}
	virtual bool endTraversal(ioc::AstNode * node) {	indent--; return true;}
};


int main(int argc,const char * argv[])
{
	llvm::sys::PrintStackTraceOnErrorSignal();
	llvm::PrettyStackTraceProgram X(argc, argv);

	ioc::Runtime::instance();
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

			std::string ext = boost::filesystem::ioc_ext::get_extension(*it);
			if(boost::iequals(ext,".xml"))
			{
			    boost::shared_ptr<ioc::frontend::XMLParser> pParser;
				pParser.reset(new ioc::frontend::XMLParser());
				if(pParser->parser(*it))
				{
				}
			}else if(boost::iequals(ext,".js")){
				boost::shared_ptr<ioc::frontend::Parser> pParser;
				pParser.reset(new ioc::frontend::Parser());
				if(pParser->parser(*it))
				{
					if(ioc::utils::Option::instance().is_existed("system.genxml"))
					{
						std::string     result;
						ioc::utils::Option::instance().getOutputFile(result,*it,"xml");

						ioc::XmlWriteVisit  visit;
						visit.WriteTo(pParser->getRootAst(),result);
					}
					visitorAAA	v;
					v.apply(pParser->getRootAst());
				}
			}else{
				std::cout << "invalid suffix '" << ext << "',";
			}
			std::cout << "done" << std::endl;
			it++;
		}
	}
	IOC_LOG_SEV(_ERROR) << "THIS IS A TEST";
	printf("hello,finish");
	return 0;
}
