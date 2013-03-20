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

#ifndef IOC_FRONTEND_ASTFACTORY_H
#define IOC_FRONTEND_ASTFACTORY_H

/**
 * @file        astfactory.h
 * @brief       作为AstNode类树的类工厂，负责创建具体类型的AstNode。
 * @relate      
 **/
 
#include 	"frontend/ast.h"

namespace ioc
{

	/** @brief 作为AstNode类树的类工厂，负责创建具体类型的AstNode。
	 */
	class AstFactory
	{
	public:
		static	AstNode*	createAstNode(IOCASTTYPES type);
	};
}

#endif // IOC_FRONTEND_ASTFACTORY_H
