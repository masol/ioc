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

#ifndef  IOC_FRONTEND_CODEGEN_H
#define  IOC_FRONTEND_CODEGEN_H

/**
 * @file     codegen.h
 * @brief    遍历ast并产生llvm instruction.
 * @details  变量使用delay的方式插入到合适的位置。codegen尝试取代varvisit.并将变量分析及优化以
 * 	         llvm pass的方式执行。这允许多线程处理不再依赖javascript前端。
 **/

#include "frontend/astvisitor.h"
#include "utils/zone.h"

namespace ioc{

class Callback;
class NameSapce;
class Variant;

class CodeGen : public AstVisitor
{
private:
	///@brief 名称空间栈，遍历AST时利用这个栈来确定变量可见区域，进而确定变量的链接。
	ZoneVector<NameSapce*>	m_nsStack;
public:
    CodeGen(NameSapce* root = 0l)
    {
        if(root)
            m_nsStack.push_back(root);
    }
    ~CodeGen()
    {
    }
    virtual void apply(AstNode *root);
protected:
	virtual bool beginTraversal(AstNode * node);
	virtual bool endTraversal(AstNode * node);
};

} //end namespace ioc.

#endif	//IOC_FRONTEND_CODEGEN_H
