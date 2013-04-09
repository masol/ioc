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
#include "frontend/xmlparser.h"
#include <boost/scope_exit.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <iostream>

namespace ioc{
namespace frontend{


bool
XMLParser::parser(const std::string& fname)
{
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
        return parser(buffer,length-1,fname.c_str());
    }
  }
  return false;
}

bool
XMLParser::processDeclaration(rapidxml_node_type *pNode)
{
    IOC_ASSERT(pNode->type() == boost::property_tree::detail::rapidxml::node_declaration);
    bool    bok = true;
	rapidxml_attr_iterator it = rapidxml_attr_iterator::begin(pNode);
    while(it != rapidxml_attr_iterator::end())
    {
        if(boost::iequals(it->name(),"encoding"))
        {
            if(!boost::iequals(it->value(),"utf-8"))
            {//@FIXME:encoding support.
                std::cerr << "Warning: encoding '" << it->value() << "' not supported." << std::endl;
                bok = false;
            }
            m_encoding = it->value();
        }
   		it++;
    }
    return bok;
}

bool
XMLParser::processPI(rapidxml_node_type *pNode)
{
	bool	bOK = false;
    IOC_ASSERT(pNode->type() == boost::property_tree::detail::rapidxml::node_pi);
    if(boost::iequals(pNode->name(),"include"))
    {
		rapidxml_attr_iterator it = ioc::rapidxml::find_attr(pNode,"src");
		while(it != rapidxml_attr_iterator::end())
        {
   			bOK = true;
            std::cout << "include src '" <<it->value() << "'" << std::endl;
            it = ioc::rapidxml::find_attr(++it,"src");
        }
    }else if(boost::iequals(pNode->name(),"namespace"))
    {
		rapidxml_attr_iterator it = ioc::rapidxml::find_attr_startwith(pNode,"xmlns:");
		while(it != rapidxml_attr_iterator::end())
        {
   			bOK = true;
            std::cout << "include namespace '" <<it->name() << "'=" << it->value() << std::endl;
            it = ioc::rapidxml::find_attr_startwith(++it,"xmlns:");
        }
    }
    return bOK;
}


bool
XMLParser::processNode(rapidxml_node_type *pNode)
{
    bool    bOK = false;
    switch(pNode->type())
    {
    case boost::property_tree::detail::rapidxml::node_declaration:
        bOK = processDeclaration(pNode);
        break;
    case boost::property_tree::detail::rapidxml::node_doctype:
        //ignore DOCTYPE now.
        bOK = true;
        break;
    case boost::property_tree::detail::rapidxml::node_pi:
        //process Process Instruction.
        break;
    case boost::property_tree::detail::rapidxml::node_comment:
        //ignore comment now.
        bOK = true;
        break;
    case boost::property_tree::detail::rapidxml::node_cdata:
        break;
    case boost::property_tree::detail::rapidxml::node_data:
        break;
    case boost::property_tree::detail::rapidxml::node_element:
        break;
    default:
        bOK = false;
        IOC_ASSERT(false && "invalid node type.");
        break;
    }
    return bOK;
}


bool
XMLParser::parser(const char* buffer,int size,const char* name)
{
    bool    bOK = false;
    boost::property_tree::detail::rapidxml::xml_document<char>    doc;
    const int flag = boost::property_tree::detail::rapidxml::parse_declaration_node | boost::property_tree::detail::rapidxml::parse_doctype_node | boost::property_tree::detail::rapidxml::parse_pi_nodes;
    try{
		///@FIXME: first get encoding info.
		///then convert to utf-8.
        doc.parse<flag>(const_cast<char*>(buffer));
		rapidxml_node_type *child = doc.first_node();
		while(child)
        {
            bOK = processNode(child);
            if(!bOK)
                break;
            child = child->next_sibling();
        }
        //pRet = readFromNode(doc.first_node(),index_dummy);
    }catch(std::exception &e)
    {
        (void)e;
    }
    return bOK;
}




}//namespace frontend
}//namespace ioc

