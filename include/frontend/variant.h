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
public:
    /// @brief Data type used in AST.
    typedef enum {
        T_INVALID,
        T_VARIANT,
        T_VOID,
        T_BOOLEAN,        // bool
        T_I2,
        T_I4,
        T_UCHAR,
        T_CHAR,            // char
        T_USHORT,
        T_SHORT,        // short
        T_UINT,
        T_INT,            // int
        T_ULONG,
        T_LONG,            // long
        T_ULONGLONG,
        T_LONGLONG,        // long long
        T_FLOAT,        // float
        T_DOUBLE,        // double
        T_LONG_DOUBLE,
        T_STRING,
        T_STRUCT,
        T_ARRAY,
        T_POINTER,
        T_ARGLIST,
        T_FUNCTION,
        T_EXTERNAL,
        T_I_N,
        T_UI128,
        T_I128,
        T_ENUM,
        T_UNION,
        T_VECTOR,
        T_OPAQUE
    } VariableType;
private:
	std::string		    m_name;
	int				        m_type;
	bool                    m_bExteranl;
	ZoneVector<NameSapce*>	m_readBlocks;
	ZoneVector<NameSapce*>	m_writeBlocks;
	typedef	boost::unordered_map<std::string,Variant>		type_variant_child_map;
	type_variant_child_map		m_children;
public:
    Variant(const std::string& name) : m_name(name), m_type(T_INVALID), m_bExteranl(false)
    {}
    inline bool  isExternal()const {return m_bExteranl;}
    inline void  markExternal(){m_bExteranl = true;}
	/// @brief if true,this is a stack variant.
	bool	isUsedInSameThread();
	inline const std::string& name(){return m_name;}
};


}



#endif //IOC_FRONTEND_VARIANT_H
