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
 
#ifndef  IOC_FRONTEND_ASTVISITOR_H
#define  IOC_FRONTEND_ASTVISITOR_H

/**
 * @file     astvisitor.h
 **/
 
 #include "utils/zone.h"
 #include "frontend/astpath.h"

namespace ioc{
	
	class AstNode;
	
	class AstVisitor : ZoneObject
	{
	protected:
		AstPath		m_currentPath;

		//we can not instantiation AstVisitor.
		AstVisitor(void){}
	private:
		inline	void	internalBeginTraversal(AstNode * node){
			m_currentPath.append(node);
			this->beginTraversal(node);
		}
		inline	void	internalEndTraversal(AstNode * node){
			this->endTraversal(node);
			m_currentPath.pop();
		}
	protected:
		friend class AstNode;
		/// @brief 派生类应该派生下面两个方法以实现具体的遍历行为。
		virtual void beginTraversal(AstNode * node) { } 
		virtual void endTraversal(AstNode * node) {	}
	public:
		~AstVisitor(void){}
		
		AstPath&	getCurrentPath(){
			return m_currentPath;
		}
		
		virtual void apply(AstNode *root);
		virtual void apply(AstPath &path){
			AstPath::iterator beginit = path.begin();
			AstPath::iterator endit = path.end();
			while(beginit != endit){
				internalBeginTraversal(*beginit);
				internalEndTraversal(*beginit);
				beginit++;
			}
		}
	};
	
} //end namespace ioc.

#endif //IOC_FRONTEND_ASTVISITOR_H
 
