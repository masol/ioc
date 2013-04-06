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

#ifndef  IOC_FRONTEND_NAMESPACE_H
#define  IOC_FRONTEND_NAMESPACE_H

#include "utils/zone.h"

namespace ioc{

class Variant;
class AstNode;
//class AssignInfo;
class NameSapce : public ioc::ZoneObject {
private:
	ZoneVector<Variant*>	m_declaredVariant;
	ZoneVector<Variant*>	m_readList;
	ZoneVector<Variant*>	m_writeList;
	ZoneVector<NameSapce*>	m_children;
	/// @brief 保存了创建本名称空间的原始ast节点。
	AstNode*				m_srcAst;
public:
    NameSapce(AstNode * node) : m_srcAst(node)
    {}
    ~NameSapce(){}
    inline void addVariant(Variant* pv){
        m_declaredVariant.push_back(pv);
    }
    Variant*   findVariant(const std::string &name);
    inline void addChild(NameSapce* ns){
        m_children.push_back(ns);
    }
};


}



#endif //IOC_FRONTEND_NAMESPACE_H
