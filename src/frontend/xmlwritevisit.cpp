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
#include "frontend/astfactory.h"
#include "utils/strext.h"
#include <boost/property_tree/detail/rapidxml.hpp>
#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>

namespace ioc{


static const char* sv_pos_prop_name = "pos";


// if ofile not initionlizer, exception throwed.
bool
XmlWriteVisit::beginTraversal(AstNode * node)
{
    OutIndent();
    m_ofstream << '<' << node->printable_type_name() ;
    int attrCount = node->attributeCount();
    AstNode *pParent = this->getCurrentPath().getNodeFromTail(1);
    if(pParent)
    {//输出位置属性。
        int count = pParent->childrenCount();
        int pos = -1;
        for(int i = 0; i < count; i++)
        {
            if(pParent->getChildren(i) == node)
            {
                pos = i;
                break;
            }
        }
        if(pos >= 0)
        {
            m_ofstream << ' '<< sv_pos_prop_name << "='" << pos << '\'';
        }
    }
    for(int i = 0; i < attrCount; i++)
    {
        const char* name;
        const char* value;
        if(node->getAttribute(i,name,value))
        {
            m_ofstream << ' '<< name << "='" << strext::encodeForXML(value) << '\'';
        }
    }
    m_ofstream  << '>' << std::endl;
    m_indent++;
	return true;
}

bool
XmlWriteVisit::endTraversal(AstNode * node)
{
    m_indent--;
    OutIndent();
    m_ofstream << "</" << node->printable_type_name() << '>' << std::endl;
	return true;
}

bool
XmlWriteVisit::WriteTo(AstNode *proot,const std::string &f)
{
    if(proot)
    {
		m_ofstream.open(f.c_str());
        if(m_ofstream.is_open())
        {
			BOOST_SCOPE_EXIT( (&m_ofstream))
			{
				m_ofstream.close();
			}BOOST_SCOPE_EXIT_END
            m_ofstream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
            this->apply(proot);
            return true;
        }
    }
    return false;
}

typedef	boost::property_tree::detail::rapidxml::xml_node<char>	node_type;
static	ioc::AstNode* readFromNode(node_type *pNode,int &index)
{
	ioc::AstNode* pAst = ioc::AstFactory::createAstNode(pNode->name());
	if(pAst)
	{
		typedef	boost::property_tree::detail::rapidxml::xml_attribute<char>		attr_type;
		attr_type* pAttr = pNode->first_attribute();
		while(pAttr)
		{
		    if(strcmp(pAttr->name(),sv_pos_prop_name) == 0)
            {
                index = boost::lexical_cast<int>(pAttr->value());
            }else if(!pAst->setAttribute(pAttr->name(),pAttr->value()))
			{
				//IOC_ASSERT(false && "invalid format");
				return NULL;
			}
			pAttr = pAttr->next_attribute();
		}
		node_type *child = pNode->first_node();
		while(child)
		{
		    int idx_cache = -1;
		    ioc::AstNode *pAstChild = readFromNode(child,idx_cache);
		    if(idx_cache >= 0)
            {
                pAst->setChildren(idx_cache,pAstChild);
            }else{
                pAst->push_back(pAstChild);
            }
			child = child->next_sibling();
		}
	}
	return pAst;
}

AstNode*
XmlWriteVisit::ReadFrom(const std::string &fname)
{
  AstNode* pRet = NULL;
  std::ifstream file(fname.c_str(),std::ifstream::binary);
  if (file) {
	BOOST_SCOPE_EXIT( (&file))
	{
		file.close();
	}BOOST_SCOPE_EXIT_END
    // get length of file:
    file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);

    char * buffer = new char [length + 1];
	BOOST_SCOPE_EXIT( (buffer))
	{
		delete[] buffer;
	}BOOST_SCOPE_EXIT_END
    // read data as a block:
    file.read (buffer,length);
    if(file)
    {
        // ...buffer contains the entire file...
        buffer[length] = 0;
        boost::property_tree::detail::rapidxml::xml_document<char>    doc;
        const int flag = boost::property_tree::detail::rapidxml::parse_no_element_values | boost::property_tree::detail::rapidxml::parse_trim_whitespace;
		try{
			doc.parse<flag>(buffer);
			int index_dummy;
			pRet = readFromNode(doc.first_node(),index_dummy);
		}catch(std::exception &e)
		{
			(void)e;
		}
    }
  }
  return pRet;
}



} //end namespace ioc.
