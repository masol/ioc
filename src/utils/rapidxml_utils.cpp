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
#include "utils/rapidxml_utils.h"
#include <boost/local_function.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace ioc{
namespace rapidxml{

rapidxml_attr_iterator  find_attr(rapidxml_attr_iterator it_begin,const char* name)
{
    if(it_begin == rapidxml_attr_iterator::end())
        return it_begin;

	bool BOOST_LOCAL_FUNCTION(bind& name,rapidxml_attr_iterator::const_reference ref) {
        return boost::iequals(ref.name(),name);
    }BOOST_LOCAL_FUNCTION_NAME(pred)

	return std::find_if(it_begin,rapidxml_attr_iterator::end(),pred);
}

rapidxml_attr_iterator  find_attr(rapidxml_node_type *pNode,const char* name)
{
    return find_attr(rapidxml_attr_iterator::begin(pNode),name);
}

rapidxml_attr_iterator  find_attr_startwith(rapidxml_node_type *pNode,const char* name)
{
    return find_attr_startwith(rapidxml_attr_iterator::begin(pNode),name);
}

rapidxml_attr_iterator  find_attr_startwith(rapidxml_attr_iterator it_begin,const char* name)
{
    if(it_begin == rapidxml_attr_iterator::end())
        return it_begin;

	bool BOOST_LOCAL_FUNCTION(bind& name,rapidxml_attr_iterator::const_reference ref) {
        return boost::istarts_with(ref.name(),name);
    }BOOST_LOCAL_FUNCTION_NAME(pred)

	return std::find_if(it_begin,rapidxml_attr_iterator::end(),pred);
}


}//rapidxml
}//ioc

