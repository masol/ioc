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
#include "frontend/namespace.h"
#include "frontend/callback.h"
#include "frontend/variant.h"

namespace ioc{


void
SRTVisit::namespaceBegin(AstNode * node)
{
    NameSapce*  cur = this->currentNamespace();

    NameSapce*  pns =new NameSapce(node);
    m_nsStack.push_back(pns);

    if(cur)
    {
        cur->addChild(pns);
    }
}

void
SRTVisit::namespaceEnd()
{
    if(m_nsStack.size())
    {
        m_nsStack.pop_back();
    }
}

Variant*
SRTVisit::findVariant(const std::string& name)
{
    return NULL;
}


void
SRTVisit::processVariant(VariableProxy* pVarAst)
{
    NameSapce*  cur = this->currentNamespace();
    Variant *pVar;
    if(pVarAst->isDeclaration())
    {
		pVar = new Variant(pVarAst->js_identifier());
		cur->addVariant(pVar);
    }else{
        Variant *pVar = this->findVariant(pVarAst->js_identifier());
        if(!pVar)
        {
            pVar = new Variant(pVarAst->js_identifier());
            cur->addVariant(pVar);
        }
    }
    IOC_ASSERT(pVar && "Invalid Variant");

    //backtrace to find variant context.
    AstPath& path = this->getCurrentPath();
    size_t depth = path.size();
    bool bBreak = false;
    for(size_t i = 0; i < depth ; i++)
    {
        AstNode* pNode = path.getNodeFromTail(i);
        switch(pNode->node_type())
        {
        case IocAst_kArguments:
            //传递给函数调用。标示本变量为外部变量。
            bBreak = true;
            break;
        case IocAst_kAssignment:
            //出现在赋值语句中。
            bBreak = true;
            break;
        case IocAst_kMultiPropertyAccessor:
            break;
        }
        if(bBreak)
            break;
    }


//step1 : check is declare. if no ,found or create in namespace.
//step2 : resolve linkage then add to variant table.

}



// if ofile not initionlizer, exception throwed.
bool
SRTVisit::beginTraversal(AstNode * node)
{
	switch(node->node_type())
	{
    //new namespace types.
    case IocAst_kSourceElements:
        this->namespaceBegin(node);
        break;
    case IocAst_kCall:
        {
            Call* pcall = node->AsCall();
        }
        break;
    case IocAst_kVariableProxy:
        this->processVariant(node->AsVariableProxy());
        break;
	}
	return true;
}

bool
SRTVisit::endTraversal(AstNode * node)
{
	switch(node->node_type())
	{
    //new namespace types.
    case IocAst_kSourceElements:
        this->namespaceEnd();
        break;
	}
	return true;
}

} //end namespace ioc.
