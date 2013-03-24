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

#ifndef  IOC_FRONTEND_ASTPATH_H
#define  IOC_FRONTEND_ASTPATH_H

/**
 * @file     astpath.h
 * @brief    AstPath负责维护AstVisitor遍历AstNode的过程中形成的路径。
 * @details  AstNode可以不维护其父节点的信息，这些信息只有在遍历中才会需要，而遍历中的AstPath维护了这些信息。
 **/
#include "utils/zone.h"
#include <vector>

namespace ioc{

	class AstNode;

	/**
	 * @brief 可以把AstPath当作标准STL中的vector<AstNode*>来用，只不过增加了几个便利方法，并且结合zone分配策略不再减少尺寸——zone本身不会释放内存。
	 */
	class AstPath : public std::vector<AstNode*, ZoneAllocator<AstNode*> > ,
					public ZoneObject
	{
	private:
		typedef std::vector<AstNode*, ZoneAllocator<AstNode*> >	inherited;
		size_type				m_depth;
	public:
		/// for compatible with stl.
		typedef inherited::value_type					value_type;
		typedef inherited::pointer						pointer;
		typedef inherited::const_pointer				const_pointer;
		typedef inherited::reference					reference;
		typedef inherited::const_reference				const_reference;
		typedef inherited::iterator						iterator;
		typedef inherited::const_iterator				const_iterator;
		typedef inherited::const_reverse_iterator		const_reverse_iterator;
		typedef inherited::reverse_iterator				reverse_iterator;
		typedef inherited::size_type					size_type;
		typedef inherited::difference_type				difference_type;
		typedef inherited::allocator_type				allocator_type;

		AstPath(void) : m_depth(0){}
		AstPath(int reservPathLength) :inherited(reservPathLength),m_depth(0){}
		AstPath(AstNode *head) : m_depth(0){
			append(head);
		}

		AstPath(const AstPath & rhs) : inherited(rhs) , m_depth(rhs.m_depth) {}
		AstPath & operator=(const AstPath & rhs){
			*dynamic_cast<inherited*>(this) = rhs;
			m_depth = rhs.m_depth;
			return *this;
		}


		void setHead(AstNode * head){
			if(inherited::size()){
				inherited::at(0) = head;
			}else{
				inherited::push_back(head);
				m_depth++;
			}
		}

		inline size_type	size() const{
			return m_depth;
		}

		void append(AstNode * node){
			if(inherited::size() > m_depth)
			{
				inherited::at(m_depth++) = node;
			}else{
				inherited::push_back(node);
				m_depth++;
			}
		}

		void append(const AstPath &frompath){
			if( frompath.size() + size() > inherited::size() )
			{
				inherited::resize(frompath.size() + size());
			}
			std::copy(frompath.begin(),frompath.begin() + frompath.size(),inherited::begin() + size());
		}

		inline void push(AstNode * node){
			append(node);
		}

		void resize(size_type __new_size, value_type __x = value_type())
		{
			if(__new_size > inherited::size())
			{
				inherited::resize(__new_size,__x);
				m_depth = __new_size;
			}else{
				m_depth = __new_size;
			}
		}

		void pop(void){
			m_depth--;
		}

		iterator end()
		{ return inherited::begin() + size();}

		const_iterator	end() const
		{ return inherited::begin() + size(); }

		AstNode * getHead(void) const
		{
			return inherited::size() ? inherited::at(0) : NULL;
		}

		AstNode * getTail(void) const
		{
			return inherited::size() ? inherited::at(m_depth - 1) : NULL;
		}

		AstNode * getNode(const int index) const{
			return (index < (int)m_depth) ? inherited::at(index) : NULL;
		}
		AstNode * getNodeFromTail(const int index) const{
			return (index < (int)m_depth) ? inherited::at(m_depth - index - 1) : NULL;
		}

	};

} //end namespace ioc.

#endif //IOC_FRONTEND_ASTPATH_H
