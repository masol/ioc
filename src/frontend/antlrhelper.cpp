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
#include "frontend/antlrhelper.h"
#include "frontend/astfactory.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

static inline void	AsignSrcinfoToNode(ioc::AstNode* pnode,ioc_src_info *psi)
{
	pnode->line(psi->m_line);
	pnode->pos((psi->m_column < 0) ? 0 : psi->m_column);
}

static inline ioc::AstNode*	GetAstNode(AST_NODE_HANDLE handle)
{
	return reinterpret_cast<ioc::AstNode*>(handle);
}

static inline AST_NODE_HANDLE	GetAstHandle(ioc::AstNode* node)
{
	return reinterpret_cast<AST_NODE_HANDLE>(node);
}

static inline void ANTLRSTR2STDSTR(pANTLR3_COMMON_TOKEN token, std::string &str)
{
	pANTLR3_STRING antlrStr = token->getText(token);                  \
	str = (char*)antlrStr->chars;
}


#ifdef IOC_DEBUG
	// Check if the argument of current function is NULL.
	#define IOC_CHECK_ARG(argName, funcName)										\
		if(!argName)																\
		{																			\
			IOC_LOG_SEV(DEBUG) << "`" << #argName << "` is NULL "					\
				<< "in `IOC_" << funcName << "()` arguments.";						\
			return NULL;															\
		}
#else
	#define IOC_CHECK_ARG(argName, funcName)		{}
#endif

/**
 * 1. Function name : `IOC_Create%name%`
 *   Create a node, his children count is const.
 * 2. Function name : `IOC_Append%name%`
 *   Create a node, his children is in a vector, append them every calling.
 **/

/**
 * @brief Sometimes ROOT of AST, but sometimes only a block in If Statement.
 * @param location
 * @return A handle on ioc::SourceElements
 * @details
 *   JavaScript: `sourceElements` parser rule in grammar.
 *     Example: "a=1; b=2; c=3"
 **/
AST_NODE_HANDLE IOC_CreateSourceElements(
	ioc_src_info *srcInfo,
	const char *fileName
)
{
	ioc::SourceElements* pSourceElements = 
		(ioc::SourceElements*)ioc::AstFactory::createAstNode(IocAst_kSourceElements);
	AsignSrcinfoToNode(pSourceElements,srcInfo);
	pSourceElements->setSourceName(fileName);
	return GetAstHandle(pSourceElements);
}

AST_NODE_HANDLE IOC_AppendSourceElements(
	AST_NODE_HANDLE srcEle,
	AST_NODE_HANDLE ele
) {
	ioc::SourceElements* pContainer = 
		GetAstNode(srcEle)->AsSourceElements();

	if(pContainer)
	{
		pContainer->push_back(GetAstNode(ele));
		return srcEle;
	}
	return NULL;
}

/**
 * @brief `functionDeclaration` parser rule in grammar.
 * @details js example "function fn(a){a=1};"
 **/
AST_NODE_HANDLE IOC_CreateFunctionDeclaration(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN name,
	AST_NODE_HANDLE param,
	AST_NODE_HANDLE body
) {
	ioc::FunctionDeclaration* pFunctionDeclaration = 
		(ioc::FunctionDeclaration*)ioc::AstFactory::createAstNode(IocAst_kFunctionDeclaration);

	AsignSrcinfoToNode(pFunctionDeclaration,srcInfo);

	// A function expression could have no function name.
	std::string str;
	if(name) {
		ANTLRSTR2STDSTR(name,str);
	} else {
		str = "";
	}
	pFunctionDeclaration->name(str);

	pFunctionDeclaration->param(GetAstNode(param));
	pFunctionDeclaration->body(GetAstNode(body));
	
	return GetAstHandle(pFunctionDeclaration);
}


/**
 * @brief `functionExpression` parser rule in grammar.
 * @details js example "function(a){a=1}" in "var fn=function(a){a=1};"
 **/
AST_NODE_HANDLE IOC_CreateFunctionExpression(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN nameToken,
	AST_NODE_HANDLE param,
	AST_NODE_HANDLE body
)
{
	ioc::FunctionExpression* pFunctionExpression = (ioc::FunctionExpression*)ioc::AstFactory::createAstNode(IocAst_kFunctionExpression);

	AsignSrcinfoToNode(pFunctionExpression,srcInfo);

	// A function expression could have no function name.
	std::string str;
	if(nameToken) {
		ANTLRSTR2STDSTR(nameToken, str);
	}
	
	pFunctionExpression->name(str);
	pFunctionExpression->param(GetAstNode(param));
	pFunctionExpression->body(GetAstNode(body));

	return GetAstHandle(pFunctionExpression);
}

/**
 * @brief Arguments of function define.
 * @example "(a,b,c)" in "function(a,b,c)"
 **/
/// @brief Only create a parent node without child.
AST_NODE_HANDLE IOC_CreateFormalParameterList(
	ioc_src_info *srcInfo
) {
	// Create parent node.
	ioc::FormalParameterList* pFormalParameterList = 
		(ioc::FormalParameterList*)ioc::AstFactory::createAstNode(IocAst_kFormalParameterList);

	AsignSrcinfoToNode(pFormalParameterList,srcInfo);
	return GetAstHandle(pFormalParameterList);
}

/// @brief Append a child to parent node.
AST_NODE_HANDLE IOC_AppendFormalParameterList(
	ioc_src_info *srcInfo,
	AST_NODE_HANDLE self,
	pANTLR3_COMMON_TOKEN identifier
) {
	// Append child to parent node.
	ioc::FormalParameterList* pContainer = GetAstNode(self)->AsFormalParameterList();
	if(pContainer)
	{
		// Create a child containing Identifier.
		ioc::VariableProxy* pVariableProxy =
			(ioc::VariableProxy*)ioc::AstFactory::createAstNode(IocAst_kVariableProxy);

		AsignSrcinfoToNode(pVariableProxy,srcInfo);
		pANTLR3_STRING str = identifier->getText(identifier);
		pVariableProxy->js_identifier((char*)str->chars);
	
		pContainer->push_back(GetAstNode(pVariableProxy));
		
		return self;
	}
	return NULL;
}

/**
 * @brief Declare a list of variables.
 * @example
 *   JavaScript: "a=1,b,c=2,d,e,f=3" in "var a=1,b,c=2,d,e,f=3"
 **/
AST_NODE_HANDLE IOC_CreateVariableDeclarationList(
	ioc_src_info *srcInfo
) {
	ioc::VariableDeclarationList* pVariableDeclarationList = 
		(ioc::VariableDeclarationList*)ioc::AstFactory::createAstNode(IocAst_kVariableDeclarationList);

	AsignSrcinfoToNode(pVariableDeclarationList,srcInfo);

	return GetAstHandle(pVariableDeclarationList);
}

AST_NODE_HANDLE IOC_AppendVariableDeclarationList(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE variableDeclaration
) {
	ioc::VariableDeclarationList* pContainer = GetAstNode(self)->AsVariableDeclarationList();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(variableDeclaration));
		return self;
	}
	return NULL;
}


/**
 * @brief `variableDeclaration` parser rule in grammar.
 * @details \
 * Examples:
 *  "a=1" in "var a=1,b,c=2"
 *  OR
 *  "b" in  "var a=1,b,c=2"
 **/
AST_NODE_HANDLE IOC_CreateVariableDeclaration_WithVar(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN identifier,
	AST_NODE_HANDLE initialiser
) {
	// Variable name.
	pANTLR3_STRING antlr_string = identifier->getText(identifier);
	char* sp_char = (char*)antlr_string->chars;
	std::string spVariableName = std::string(sp_char);

	// Create a child containing Identifier, AKA left value.
	ioc::VariableProxy* pVariableProxy;
	pVariableProxy = (ioc::VariableProxy*)ioc::AstFactory::createAstNode(IocAst_kVariableProxy);

	pVariableProxy->js_identifier(spVariableName);
	pVariableProxy->isDeclaration(ANTLR3_TRUE);
	AsignSrcinfoToNode(pVariableProxy,srcInfo);

	ioc::Assignment* pAssignment = (ioc::Assignment*)ioc::AstFactory::createAstNode(IocAst_kAssignment);
	AsignSrcinfoToNode(pAssignment,srcInfo);
		
	pAssignment->operater(ioc::Assignment::T_EQU); // Must be '='.
	pAssignment->left(GetAstNode(pVariableProxy));
	pAssignment->right(GetAstNode(initialiser));
		
	return GetAstHandle(pAssignment);
}


/**
 * @brief `Identifier` parser rule in grammar.
 * @example "foo" in "var foo;" OR "var foo=1;"
 **/
AST_NODE_HANDLE IOC_CreateVariableProxy_WithVar(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN identifier
) {
	ioc::VariableProxy* pVariableProxy =
		(ioc::VariableProxy*)ioc::AstFactory::createAstNode(IocAst_kVariableProxy);
		
	AsignSrcinfoToNode(pVariableProxy,srcInfo);

	std::string str((char*)identifier->getText(identifier)->chars);
	pVariableProxy->js_identifier(str);
	pVariableProxy->isDeclaration(ANTLR3_TRUE);
	
	return GetAstHandle(pVariableProxy);
}

/**
 * @brief `emptyStatement` parser rule in grammar.
 * Describ a blank line which only has a single semicolon.
 * @example ";"
 **/
AST_NODE_HANDLE IOC_CreateEmptyStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token
) {
	ioc::EmptyStatement* pEmptyStatement = 
		(ioc::EmptyStatement*)ioc::AstFactory::createAstNode(IocAst_kEmptyStatement);
	AsignSrcinfoToNode(pEmptyStatement,srcInfo);
	return GetAstHandle(pEmptyStatement);
}


/**
 * @brief `ifStatement` parser rule in grammar.
 * @example "if(a==1){b=1}" OR "if(a==1){b=1}else{b=2}"
 * "else if()" OR "elseif()" are just like "else{if()}"
 **/
AST_NODE_HANDLE IOC_CreateIfStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN ifToken,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE s1,
	AST_NODE_HANDLE s2
) {
	ioc::IfStatement* pIfStatement = 
		(ioc::IfStatement*)ioc::AstFactory::createAstNode(IocAst_kIfStatement);
	AsignSrcinfoToNode(pIfStatement,srcInfo);

	pIfStatement->conditionExpression(GetAstNode(e));
	pIfStatement->thenStatement(GetAstNode(s1));
	
	// If there is no "else" statement, it is just like "else `emptyStatement`"
	if(s2) {
		pIfStatement->elseStatement(GetAstNode(s2));
	} else {
		ioc::EmptyStatement* pEmptyStatement = 
			(ioc::EmptyStatement*)ioc::AstFactory::createAstNode(IocAst_kEmptyStatement);
		AsignSrcinfoToNode(pEmptyStatement,srcInfo);
		pIfStatement->elseStatement(GetAstNode(pEmptyStatement));
	}

	return GetAstHandle(pIfStatement);
}



#ifdef __cplusplus
}
#endif
