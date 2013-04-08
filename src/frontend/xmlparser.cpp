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
#include <boost/property_tree/detail/rapidxml.hpp>
#include <boost/scope_exit.hpp>
#include <fstream>

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


typedef	boost::property_tree::detail::rapidxml::xml_node<char>	node_type;
static	void processNode(node_type *pNode,XMLParser *pParser)
{
    switch(pNode->type())
    {
    case boost::property_tree::detail::rapidxml::node_declaration:
		//if(pParser->
        break;
    case boost::property_tree::detail::rapidxml::node_doctype:
        break;
    case boost::property_tree::detail::rapidxml::node_pi:
        break;
    case boost::property_tree::detail::rapidxml::node_comment:
        break;
    case boost::property_tree::detail::rapidxml::node_cdata:
        break;
    case boost::property_tree::detail::rapidxml::node_data:
        break;
    case boost::property_tree::detail::rapidxml::node_element:
        break;
    default:
        IOC_ASSERT(false && "invalid node type.");
    }
}


bool
XMLParser::parser(const char* buffer,int size,const char* name)
{
    boost::property_tree::detail::rapidxml::xml_document<char>    doc;
    const int flag = boost::property_tree::detail::rapidxml::parse_declaration_node | boost::property_tree::detail::rapidxml::parse_doctype_node | boost::property_tree::detail::rapidxml::parse_pi_nodes;
    try{
		///@FIXME: first get encoding info.
		///then convert to utf-8.
        doc.parse<flag>(const_cast<char*>(buffer));
		node_type *child = doc.first_node();
		while(child)
        {
            processNode(child,this);
            child = child->next_sibling();
        }
        //pRet = readFromNode(doc.first_node(),index_dummy);
    }catch(std::exception &e)
    {
        (void)e;
    }

    return false;
}




}//namespace frontend
}//namespace ioc

