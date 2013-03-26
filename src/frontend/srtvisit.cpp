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
#include "frontend/ast.h"
#include "frontend/srtvisit.h"
#include "frontend/astfactory.h"

namespace ioc{



// if ofile not initionlizer, exception throwed.
bool
SRTVisit::beginTraversal(AstNode * node)
{
	return true;
}

bool
SRTVisit::endTraversal(AstNode * node)
{
	return true;
}

} //end namespace ioc.
