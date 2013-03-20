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

#include "frontend/astfactory.h"

namespace ioc
{
	AstNode*	AstFactory::createAstNode(IOCASTTYPES type)
	{
		AstNode* ret = NULL;
		
#define CREATE_TYPE_INSTANCE(type) \
	case IocAst_k##type:	\
		ret = new type();	\
		break;
		
		switch(type){
			AST_NODE_LIST(CREATE_TYPE_INSTANCE)
		//prevent compiler error.
		default:
			break;
		}
		return ret;
	}
}