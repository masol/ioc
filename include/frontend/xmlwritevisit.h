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

#ifndef  IOC_FRONTEND_ASTWRITEVISIT_H
#define  IOC_FRONTEND_ASTWRITEVISIT_H

/**
 * @file     xmlwritevisit.h
 * @brief    遍历ast以生成xml文件。
 **/

#include "frontend/astvisitor.h"
#include <fstream>

namespace ioc{

class XmlWriteVisit : public AstVisitor
{
private:
    virtual void    beginTraversal(AstNode * node);
    virtual void    endTraversal(AstNode * node);
    std::ofstream   m_ofstream;
    int              m_indent;
    inline  void    OutIndent(void){
        for(int i = 0 ; i < m_indent; i++)
        {
            this->m_ofstream << "    ";
        }
    }
public:
    XmlWriteVisit() : m_indent(0)
    {
    }
    ~XmlWriteVisit()
    {
        if(m_ofstream.is_open())
            m_ofstream.close();
    }
    //@todo encoding process.
    bool   WriteTo(AstNode *proot,const std::string &f);

    static  AstNode*  ReadFrom(const std::string &fname);
};

} //end namespace ioc.

#endif	//IOC_FRONTEND_ASTWRITEVISIT_H
