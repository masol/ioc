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

#ifndef  IOC_FRONTEND_SRTVISIT_H
#define  IOC_FRONTEND_SRTVISIT_H

/**
 * @file     srtvisit.h
 * @brief    遍历ast并执行srt（Semantic Resolution Tree）变换。用于产生全局的变量表，块表以及线程表。
 * @details  变换成功之后，SRTVisit维护了面向变量使用的语义树(semantic resolution tree)。通过遍历对应的语法树产生语义树，我们的分析都是针对语义树展开的。
 *   SRTVisit并不处理基于变量依赖关系的代码分析与重构，这个特性留待以后实现。这允许我们发现适合并行的代码块，并产生适应特定并行框架的代码(例如opencl或者openmp).
 *   变量依赖关系分析需要重构AST结构，生成以操作为中心的AST子树集合（集合之间也有依赖关系)。所有子节点的变量依赖于父节点。通过寻找AST图中控制点的读写特性来判定是否可以转为MapReduce模式。
 **/

#include "frontend/astvisitor.h"
#include "utils/zone.h"

namespace ioc{

class Callback;
class NameSapce;
class Variant;

class SRTVisit : public AstVisitor
{
private:
	///@brief 名称空间栈，遍历AST时利用这个栈来确定变量可见区域，进而确定变量的链接。
	ZoneVector<NameSapce*>	m_nsStack;
	///@brief 保存了本段代码全部的回调函数。
	///@details 回调函数表会在以后两个环节发生作用，一是接下来的函数重组，组织为函数指针方式。二是线程分析，每个回调函数都是潜在的线程入口点。
	ZoneVector<Callback*>	m_callbackTable;
	void	namespaceBegin(AstNode * node);
	void	namespaceEnd(void);
	void	processVariant(VariableProxy* pVarAst);
	inline NameSapce*  currentNamespace(void)
	{
	    return m_nsStack.size() ? m_nsStack.front() : NULL;

	}
	Variant*    findVariant(const std::string& name);
public:
    SRTVisit()
    {
    }
    ~SRTVisit()
    {
    }
protected:
	virtual bool beginTraversal(AstNode * node);
	virtual bool endTraversal(AstNode * node);
};

} //end namespace ioc.

#endif	//IOC_FRONTEND_ASTWRITEVISIT_H
