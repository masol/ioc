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

#ifndef  IOC_FRONTEND_SSAVISIT_H
#define  IOC_FRONTEND_SSAVISIT_H

/**
 * @file     ssavisit.h
 * @brief    遍历ast并执行ssa（Static Single-Assignment）变换。
 **/

#include "frontend/astvisitor.h"

namespace ioc{

class SSAVisit : public AstVisitor
{
private:
public:
    SSAVisit()
    {
    }
    ~SSAVisit()
    {
    }
};

} //end namespace ioc.

#endif	//IOC_FRONTEND_ASTWRITEVISIT_H
