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

/// @brief Convert pANTLR3_STRING to char*
#define STR_COVERTER(antlr_string) (char*)antlr_string->chars

#define SOURCE_NAME(istream) (char*)(istream->getSourceName(istream)->chars)

/**
 * @brief Create ioc AST nodes.
 * 不用创建过度型节点。
 **/

AST_NODE_HANDLE IOC_CreateSourceElements(int location[], const char*);
AST_NODE_HANDLE	IOC_AppendSourceElements(AST_NODE_HANDLE srcEle, AST_NODE_HANDLE ele);
AST_NODE_HANDLE IOC_CreateCaseBlock(int location[]);

AST_NODE_HANDLE	IOC_CreateFunctionDeclaration(int location[], pANTLR3_COMMON_TOKEN name, AST_NODE_HANDLE param, AST_NODE_HANDLE body);
AST_NODE_HANDLE IOC_CreateFunctionExpression(int location[], pANTLR3_COMMON_TOKEN name, AST_NODE_HANDLE param, AST_NODE_HANDLE body);
AST_NODE_HANDLE IOC_SetjsFuncExpression(AST_NODE_HANDLE srcEle, const char*);

AST_NODE_HANDLE IOC_CreateFormalParameterList(int location[]);
AST_NODE_HANDLE IOC_AppendFormalParameterList(AST_NODE_HANDLE self, pANTLR3_COMMON_TOKEN token);
AST_NODE_HANDLE IOC_CreateVariableDeclarationList(int location[]);
AST_NODE_HANDLE IOC_AppendVariableDeclarationList(AST_NODE_HANDLE self, AST_NODE_HANDLE variableDeclaration);
AST_NODE_HANDLE IOC_CreateVariableDeclaration_WithVar(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateEmptyStatement(int location[], pANTLR3_COMMON_TOKEN token);
AST_NODE_HANDLE IOC_CreateIfStatement(int location[], pANTLR3_COMMON_TOKEN ifToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s1, AST_NODE_HANDLE s2);
AST_NODE_HANDLE IOC_CreateDoWhileStatement(int location[], pANTLR3_COMMON_TOKEN doToken, AST_NODE_HANDLE s, pANTLR3_COMMON_TOKEN whileToken, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateWhileStatement(int location[], pANTLR3_COMMON_TOKEN whileToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateForStatement(int location[], pANTLR3_COMMON_TOKEN forToken, AST_NODE_HANDLE i, AST_NODE_HANDLE e1, AST_NODE_HANDLE e2, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateForInStatement(int location[], pANTLR3_COMMON_TOKEN forToken, AST_NODE_HANDLE i, pANTLR3_COMMON_TOKEN inToken, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateContinueStatement(int location[], pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i);
AST_NODE_HANDLE IOC_CreateBreakStatement(int location[], pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i);
AST_NODE_HANDLE IOC_CreateReturnStatement(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateWithStatement(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateLabelledStatement(int location[], pANTLR3_COMMON_TOKEN i, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateSwitchStatement(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE b);
AST_NODE_HANDLE IOC_CreateCaseBlock(int location[]);
AST_NODE_HANDLE IOC_AppendCaseBlock(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateCaseClause(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateDefaultClause(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateThrow(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateTryStatement(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s, AST_NODE_HANDLE c, AST_NODE_HANDLE f);
AST_NODE_HANDLE IOC_CreateTryCatchStatement(int location[], pANTLR3_COMMON_TOKEN token, pANTLR3_COMMON_TOKEN i, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateTryFinallyStatement(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE s);
AST_NODE_HANDLE IOC_CreateAssignment(int location[], pANTLR3_COMMON_TOKEN op, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateNewExpression(int location[], pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateMemberExpression(int location[], char * newSrc, char * memberSrc, char * argumentsSrc, pANTLR3_COMMON_TOKEN token, AST_NODE_HANDLE m, AST_NODE_HANDLE a);
AST_NODE_HANDLE IOC_CreateCall(int location[], AST_NODE_HANDLE m, AST_NODE_HANDLE a);
AST_NODE_HANDLE IOC_CreateArguments(int location[]);
AST_NODE_HANDLE IOC_AppendArguments(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateIndexSuffix(int location[], AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreatePropertyReferenceSuffix(int location[], pANTLR3_COMMON_TOKEN token);

AST_NODE_HANDLE	IOC_CreateVariableProxy_WithVar(int location[], pANTLR3_COMMON_TOKEN identifier);
AST_NODE_HANDLE	IOC_CreateVariableProxy(int location[], pANTLR3_COMMON_TOKEN identifier);


/**
 * @brief `conditional` parser rule in grammar.
 * @details Example NULL
 **/
AST_NODE_HANDLE IOC_CreateConditional(int location[],pANTLR3_COMMON_TOKEN token1,pANTLR3_COMMON_TOKEN token2,AST_NODE_HANDLE e,AST_NODE_HANDLE e1,AST_NODE_HANDLE e2);

AST_NODE_HANDLE	IOC_CreateBinaryOperation(int location[], pANTLR3_COMMON_TOKEN operatorToken, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateCompareOperation(int location[], pANTLR3_COMMON_TOKEN operatorToken, AST_NODE_HANDLE left, AST_NODE_HANDLE right);
AST_NODE_HANDLE IOC_CreateUnaryOperation(int location[], pANTLR3_COMMON_TOKEN token, ANTLR3_BOOLEAN isFrontOp, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateObjectLiteral(int location[]);
AST_NODE_HANDLE IOC_AppendObjectLiteral(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateArrayLiteral(int location[]);
AST_NODE_HANDLE IOC_AppendArrayLiteral(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreatePropertyNameAndValue(int location[], AST_NODE_HANDLE p, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_CreateMultiPropertyAccessor(int location[]);
AST_NODE_HANDLE IOC_AppendMultiPropertyAccessor(AST_NODE_HANDLE self, AST_NODE_HANDLE e);
AST_NODE_HANDLE IOC_AppendMultiPropertyAccessor2(AST_NODE_HANDLE self, AST_NODE_HANDLE e, AST_NODE_HANDLE arg);
AST_NODE_HANDLE	IOC_CreateStringLiteral(int location[], pANTLR3_COMMON_TOKEN str);
AST_NODE_HANDLE	IOC_CreateNumberLiteral(int location[], pANTLR3_COMMON_TOKEN num);
AST_NODE_HANDLE IOC_CreateBooleanLiteral(int location[], pANTLR3_COMMON_TOKEN str);
AST_NODE_HANDLE IOC_CreateNullLiteral(int location[], pANTLR3_COMMON_TOKEN str);

AST_NODE_HANDLE IOC_CreateMap(int location[], char* i, AST_NODE_HANDLE e);
//xxdebug
int IOC_IsVariableProxy(AST_NODE_HANDLE e);

#ifdef __cplusplus
}
#endif

#endif //IOC_FRONTEND_ANTLRHELPER_H