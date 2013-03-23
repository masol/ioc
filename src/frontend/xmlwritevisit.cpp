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
#include "frontend/xmlwritevisit.h"


namespace ioc{

// if ofile not initionlizer, exception throwed.
void
XmlWriteVisit::beginTraversal(AstNode * node)
{
    OutIndent();
    m_ofstream << '<' << node->printable_type_name() << '>' << std::endl;
    m_indent++;
}

void
XmlWriteVisit::endTraversal(AstNode * node)
{
    m_indent--;
    OutIndent();
    m_ofstream << "</" << node->printable_type_name() << '>' << std::endl;
}

bool
XmlWriteVisit::WriteTo(AstNode *proot,const std::string &f)
{
    if(proot)
    {
		m_ofstream.open(f.c_str());
        if(m_ofstream.is_open())
        {
            m_ofstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
            this->apply(proot);
            return true;
        }
    }
    return false;
}


} //end namespace ioc.
