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

#ifndef IOC_FRONTEND_XMLPARSER_H
#define IOC_FRONTEND_XMLPARSER_H

/**
 * @file        xmlparser.h
 * @brief       作为语言分析前端的接口。所有语言前端实现此接口。。
 * @relate
 **/

#include	"utils/zone.h"
#include    "utils/rapidxml_utils.h"

namespace ioc{
namespace frontend{

	/** @brief parser接口
	 */
	class XMLParser
	{
	protected:
		int				m_errorLineNumber;
		int				m_errorCharNumber;
		int				m_errorCode;
		std::string    m_encoding;
        bool        processNode(rapidxml_node_type *pNode);
        bool        processDeclaration(rapidxml_node_type *pNode);
        bool        processPI(rapidxml_node_type *pNode);
	public:
		XMLParser(void) : m_errorLineNumber(-1),m_errorCharNumber(-1),m_errorCode(-1),m_encoding("utf-8")
		{
		}

		virtual ~XMLParser(void)
		{
		}

         /** @brief 分析指定的文件。每次分析会把所有结果重置。
		 *  @return true表示分析成功。可以调用getRootAst获取分析结果。
		 */
		virtual bool	parser(const std::string&);

         /** @brief 分析指定的数据缓冲区。每次分析会把所有结果重置。
		 *  @return true表示分析成功。可以调用getRootAst获取分析结果。
		 */
		virtual bool	parser(const char* buf,int size,const char* name = NULL);


		/* @brief 如果分析失败，获取失败的行数。-1表示无法获取.
		 */
		int 	getErrorLineNumber(void)const{
			return m_errorLineNumber;
		}
		/** @brief 如果分析失败，获取失败的列数。-1表示无法获取.
		 */
		int		getErrorChar(void)const{
			return m_errorCharNumber;
		}
		/** @brief 如果分析失败，获取失败的错误代码。-1表示无法获取.
		 */
		int		getErrorCode(void)const{
			return m_errorCode;
		}

	};

}//namespace frontend.
}//namespace ioc

#endif // IOC_FRONTEND_XMLPARSER_H
