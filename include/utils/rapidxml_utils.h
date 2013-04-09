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

#ifndef IOC_UTILS_RAPIDXML_UTILS_H
#define IOC_UTILS_RAPIDXML_UTILS_H

#include    <boost/property_tree/detail/rapidxml.hpp>

namespace ioc
{

typedef	boost::property_tree::detail::rapidxml::xml_node<char>	        rapidxml_node_type;
typedef	boost::property_tree::detail::rapidxml::xml_attribute<char>	rapidxml_attr_type;

class rapidxml_attr_iterator
{
public:
	typedef rapidxml_attr_iterator           _Myt;
   	typedef ptrdiff_t                        difference_type;
	typedef std::bidirectional_iterator_tag  iterator_category;
   	typedef rapidxml_attr_type               value_type;
   	typedef rapidxml_attr_type               *pointer;
   	typedef rapidxml_attr_type               &reference;
   	typedef const rapidxml_attr_type        &const_reference;
protected:
    pointer                         _MyAttr;
public:

   	rapidxml_attr_iterator(){
   	    _MyAttr = 0;
   	}
   	explicit rapidxml_attr_iterator(pointer attr){
   	    _MyAttr = attr;
   	}
   	rapidxml_attr_iterator(const rapidxml_attr_iterator& ref){
   	    _MyAttr = ref._MyAttr;
   	}
   	reference operator*() const
   	{
   	    return (*_MyAttr);
   	}
	pointer operator->() const
    {	// return pointer to class object
        return (_MyAttr);
    }
    pointer get()const{
        return (_MyAttr);
    }

    _Myt& operator++()
    {// preincrement
        _MyAttr = _MyAttr->next_attribute();
        return *this;
    }

    _Myt operator++(int)
    {// postincrement
		_Myt _Tmp = *this;
		++*this;
		return (_Tmp);
    }

    _Myt& operator--()
    {	// predecrement
        _MyAttr = _MyAttr->previous_attribute();
    }

    _Myt operator--(int)
	{	// postdecrement
		_Myt _Tmp = *this;
		--*this;
        return (_Tmp);
	}

	bool operator==(const _Myt& _Right) const
	{
	    return (_MyAttr == _Right._MyAttr);
	}

	bool operator!=(const _Myt& _Right) const
	{
	    return (_MyAttr != _Right._MyAttr);
	}

	_Myt& operator=(const _Myt& _Right)
	{
	    _MyAttr = _Right._MyAttr;
	    return *this;
	}

	inline static  rapidxml_attr_iterator  begin(rapidxml_node_type *node){
	    return rapidxml_attr_iterator(node->first_attribute());
	}

	inline static  rapidxml_attr_iterator  end(){
	    return rapidxml_attr_iterator();
	}
};

    namespace rapidxml{
        rapidxml_attr_iterator  find_attr(rapidxml_node_type *pNode,const char* name);
        rapidxml_attr_iterator  find_attr(rapidxml_attr_iterator it,const char* name);
        rapidxml_attr_iterator  find_attr_startwith(rapidxml_node_type *pNode,const char* name);
        rapidxml_attr_iterator  find_attr_startwith(rapidxml_attr_iterator it,const char* name);
    }
}

#endif //IOC_UTILS_RAPIDXML_UTILS_H
