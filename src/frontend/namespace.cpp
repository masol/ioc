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
#include "frontend/namespace.h"
#include "frontend/callback.h"
#include "frontend/variant.h"
#include <boost/local_function.hpp>

namespace ioc{


Variant*
NameSapce::findVariant(const std::string &name)
{
    bool BOOST_LOCAL_FUNCTION(const bind& name,Variant* pVar)
    {
        return (name.compare(pVar->name()) == 0);
    } BOOST_LOCAL_FUNCTION_NAME(findVar)
	
	ZoneVector<Variant*>::iterator it = std::find_if(m_declaredVariant.begin(),m_declaredVariant.end(),findVar);
	if(it != m_declaredVariant.end())
	{
		return *it;
	}
	return NULL;
}

} //end namespace ioc.
