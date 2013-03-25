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

#ifndef  IOC_FRONTEND_ANTLRHELPER_H
#define  IOC_FRONTEND_ANTLRHELPER_H

/**
 * @file        antlrhelper.h
 * @brief       antlrhelper提供了anltr.g文件在遍历时设置其节点的语句类型，以便在buildFromAntlr中更快速的产生ioc AST.
 **/

#include "frontend/asttypes.h"
#include <antlr3.h>

typedef	void*	AST_NODE_HANDLE;
#ifdef __cplusplus
extern "C" {
#endif

/// @brief set error state.so we can set the detailed error information.
#define	SET_USER_ERROR_STATE(state_value)		\
		{RECOGNIZER->state->user1 = (ANTLR3_UINT32)(state_value);}


/// @brief Convert pANTLR3_STRING to char*
#define STR_COVERTER(antlr_string) (char*)antlr_string->chars

#define SOURCE_NAME(istream) (char*)(istream->getSourceName(istream)->chars)

// we only save file name in SourceElement node.so no filename here.
typedef	struct	_tag_ioc_src_info{
	int		m_line;
	int		m_column;
}ioc_src_info;

//void	IOC_ASSIGN_SRC_INFO(pANTLR3_COMMON_TOKEN  *token,ioc_src_info *srcinfo);
#define	IOC_ASSIGN_SRC_INFO(token,srcinfo)			\
{													\
	(srcinfo).m_line = (token)->line;				\
	(srcinfo).m_column = (token)->charPosition;		\
}

#define	IOC_INIT_SRC_INFO(srcinfo)		\
{										\
	(srcinfo).m_line = 0;				\
	(srcinfo).m_column = 0;				\
}

/**
 * @brief Create ioc AST nodes.
 * 不用创建过度型节点。
 **/

AST_NODE_HANDLE IOC_CreateSourceElements(ioc_src_info *srcInfo, const char*);
AST_NODE_HANDLE	IOC_AppendSourceElements(AST_NODE_HANDLE srcEle, AST_NODE_HANDLE ele);
AST_NODE_HANDLE	IOC_CreateFunctionDeclaration(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN name, AST_NODE_HANDLE param, AST_NODE_HANDLE body);
AST_NODE_HANDLE IOC_CreateFunctionExpression(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN name, AST_NODE_HANDLE param, AST_NODE_HANDLE body);
AST_NODE_HANDLE IOC_CreateFormalParameterList(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendFormalParameterList(ioc_src_info *srcInfo,AST_NODE_HANDLE self, pANTLR3_COMMON_TOKEN token);
AST_NODE_HANDLE IOC_CreateVariableDeclarationList(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendVariableDeclarationList(AST_NODE_HANDLE self, AST_NODE_HANDLE variableDeclaration);
AST_NODE_HANDLE IOC_CreateVariableDeclaration_WithVar(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE	IOC_CreateVariableProxy_WithVar(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN identifier);
AST_NODE_HANDLE IOC_CreateEmptyStatement(ioc_src_info *srcInfo,pANTLR3_COMMON_TOKEN token);
AST_NODE_HANDLE IOC_CreateIfStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN ifToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s1, AST_NODE_HANDLE s2);
AST_NODE_HANDLE IOC_CreateDoWhileStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN doToken, AST_NODE_HANDLE s, pANTLR3_COMMON_TOKEN whileToken, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateWhileStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN whileToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateForStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN forToken, AST_NODE_HANDLE i, AST_NODE_HANDLE e1, AST_NODE_HANDLE e2, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateForInStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN forToken, AST_NODE_HANDLE i, pANTLR3_COMMON_TOKEN inToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateContinueStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i);
AST_NODE_HANDLE IOC_CreateBreakStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i);
AST_NODE_HANDLE IOC_CreateReturnStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateWithStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateLabelledStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN i, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateSwitchStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE b);
AST_NODE_HANDLE IOC_CreateCaseBlock(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendCaseBlock(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateCaseClause(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateDefaultClause(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateThrow(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateTryStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s, AST_NODE_HANDLE c, AST_NODE_HANDLE f);
AST_NODE_HANDLE IOC_CreateTryCatchStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateTryFinallyStatement(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s);
AST_NODE_HANDLE	IOC_CreateBinaryOperation(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN operatorToken, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateAssignment(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN op, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateNewExpression(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e,AST_NODE_HANDLE arg);
AST_NODE_HANDLE IOC_CreateMultiPropertyAccessor(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendMultiPropertyAccessor(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateCall(ioc_src_info *srcInfo, AST_NODE_HANDLE m, AST_NODE_HANDLE a);
AST_NODE_HANDLE IOC_CreateArguments(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendArguments(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateIndexSuffix(ioc_src_info *srcInfo, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreatePropertyReferenceSuffix(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token);
AST_NODE_HANDLE IOC_CreateConditional(ioc_src_info *srcInfo,AST_NODE_HANDLE e,AST_NODE_HANDLE e1,AST_NODE_HANDLE e2);
AST_NODE_HANDLE IOC_CreateCompareOperation(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN operatorToken, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateUnaryOperation(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN token, ANTLR3_BOOLEAN isFrontOp, AST_NODE_HANDLE e);
AST_NODE_HANDLE	IOC_CreateVariableProxy(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN identifier);
AST_NODE_HANDLE IOC_CreateArrayLiteral(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendArrayLiteral(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateObjectLiteral(ioc_src_info *srcInfo);
AST_NODE_HANDLE IOC_AppendObjectLiteral(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreatePropertyNameAndValue(ioc_src_info *srcInfo, AST_NODE_HANDLE p, AST_NODE_HANDLE e);
AST_NODE_HANDLE	IOC_CreateStringLiteral(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN str);
AST_NODE_HANDLE	IOC_CreateNumberLiteral(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN num);
AST_NODE_HANDLE IOC_CreateNullLiteral(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN str);
AST_NODE_HANDLE IOC_CreateBooleanLiteral(ioc_src_info *srcInfo, pANTLR3_COMMON_TOKEN str);

#ifdef __cplusplus
}
#endif

#endif //IOC_FRONTEND_ANTLRHELPER_H