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

#ifndef  IOC_FRONTEND_VARIANT_H
#define  IOC_FRONTEND_VARIANT_H

#include "utils/zone.h"
#include<boost/unordered_map.hpp>

namespace ioc{

class NameSapce;

class Variant : public ioc::ZoneObject
{
private:
	std::string		name;
	int				m_type;
	ZoneVector<NameSapce*>	m_readBlocks;
	ZoneVector<NameSapce*>	m_writeBlocks;
	typedef	boost::unordered_map<std::string,Variant>		type_variant_child_map;
	type_variant_child_map		m_children;
public:
	/// @brief if true,this is a stack variant.
	bool	isUsedInSameThread();
};


}



#endif //IOC_FRONTEND_VARIANT_H