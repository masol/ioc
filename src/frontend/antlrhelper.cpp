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
#include "utils/log.h"
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

	pAssignment->opName("="); // Must be '='.
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

/**
 * @brief `doWhileStatement` parser rule in grammar.
 * @example "do{b=1}while(a==1)"
 **/
AST_NODE_HANDLE IOC_CreateDoWhileStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN doToken,
	AST_NODE_HANDLE s,
	pANTLR3_COMMON_TOKEN whileToken,
	AST_NODE_HANDLE e
)
{
	ioc::DoWhileStatement* pDoWhileStatement =
		(ioc::DoWhileStatement*)ioc::AstFactory::createAstNode(IocAst_kDoWhileStatement);
	AsignSrcinfoToNode(pDoWhileStatement,srcInfo);

	pDoWhileStatement->body(GetAstNode(s));
	pDoWhileStatement->condition(GetAstNode(e));

	return GetAstHandle(pDoWhileStatement);
}


/**
 * @brief `whileStatement` parser rule in grammar.
 * @example "while(a==1){b=1}"
 **/
AST_NODE_HANDLE IOC_CreateWhileStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN whileToken,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE s
) {
	ioc::WhileStatement* pWhileStatement =
		(ioc::WhileStatement*)ioc::AstFactory::createAstNode(IocAst_kWhileStatement);
	AsignSrcinfoToNode(pWhileStatement,srcInfo);

	pWhileStatement->condition(GetAstNode(e));
	pWhileStatement->body(GetAstNode(s));

	return GetAstHandle(pWhileStatement);
}

/**
 * @brief `forStatement` parser rule in grammar.
 * @example "for(i=0;i<10;i++){echo i}"
 **/
AST_NODE_HANDLE IOC_CreateForStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN forToken,
	AST_NODE_HANDLE i,
	AST_NODE_HANDLE e1,
	AST_NODE_HANDLE e2,
	AST_NODE_HANDLE s
) {
	ioc::ForStatement* pForStatement =
		(ioc::ForStatement*)ioc::AstFactory::createAstNode(IocAst_kForStatement);
	AsignSrcinfoToNode(pForStatement,srcInfo);

	pForStatement->init(GetAstNode(i));
	pForStatement->condition(GetAstNode(e1));
	pForStatement->next(GetAstNode(e2));
	pForStatement->body(GetAstNode(s));

	return GetAstHandle(pForStatement);
}


/**
 * @brief `forInStatement` parser rule in grammar.
 * @example "for(i in arr){echo arr[i]}"
 **/
AST_NODE_HANDLE IOC_CreateForInStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN forToken,
	AST_NODE_HANDLE i,
	pANTLR3_COMMON_TOKEN inToken,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE s
) {
	ioc::ForInStatement* pForInStatement =
		(ioc::ForInStatement*)ioc::AstFactory::createAstNode(IocAst_kForInStatement);
	AsignSrcinfoToNode(pForInStatement,srcInfo);

	pForInStatement->init(GetAstNode(i));
	pForInStatement->enumerable(GetAstNode(e));
	pForInStatement->body(GetAstNode(s));

	return GetAstHandle(pForInStatement);
}


/**
 * @brief `continueStatement` parser rule in grammar.
 * @example "continue i;"
 **/
AST_NODE_HANDLE IOC_CreateContinueStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	pANTLR3_COMMON_TOKEN i
) {
	// Create parent node.
	ioc::ContinueStatement* pContinueStatement =
		(ioc::ContinueStatement*)ioc::AstFactory::createAstNode(IocAst_kContinueStatement);
	AsignSrcinfoToNode(pContinueStatement,srcInfo);

	if(i){
		// The label name.
		pANTLR3_STRING antlr_string = i->getText(i);
		char* ioc_char = (char*)antlr_string->chars;
		pContinueStatement->identifier(ioc_char);
	}

	return GetAstHandle(pContinueStatement);
}


/**
 * @brief `breakStatement` parser rule in grammar.
 * @example "break i;" OR "break;"
 **/
AST_NODE_HANDLE IOC_CreateBreakStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	pANTLR3_COMMON_TOKEN i
)
{
	// Create parent node.
	ioc::BreakStatement* pBreakStatement =
		(ioc::BreakStatement*)ioc::AstFactory::createAstNode(IocAst_kBreakStatement);
	AsignSrcinfoToNode(pBreakStatement,srcInfo);

	if(i) {
		// The label name.
		pANTLR3_STRING antlr_string = i->getText(i);
		const char* ioc_char = (char*)antlr_string->chars;
		pBreakStatement->identifier(ioc_char);
	}
	return GetAstHandle(pBreakStatement);
}


/**
 * @brief `returnStatement` parser rule in grammar.
 * @example "return i=1;" OR "return ;"
 **/
AST_NODE_HANDLE IOC_CreateReturnStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e
) {
	ioc::ReturnStatement* pReturnStatement =
		(ioc::ReturnStatement*)ioc::AstFactory::createAstNode(IocAst_kReturnStatement);
	AsignSrcinfoToNode(pReturnStatement,srcInfo);

	if(e) {
		pReturnStatement->expression(GetAstNode(e));
	}

	return GetAstHandle(pReturnStatement);
}


/**
 * @brief `withStatement` parser rule in grammar.
 * @example NULL
 **/
AST_NODE_HANDLE IOC_CreateWithStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE s
)
{
	ioc::WithStatement* pWithStatement =
		(ioc::WithStatement*)ioc::AstFactory::createAstNode(IocAst_kWithStatement);
	AsignSrcinfoToNode(pWithStatement,srcInfo);

	pWithStatement->expression(GetAstNode(e));
	pWithStatement->statement(GetAstNode(s));

	return GetAstHandle(pWithStatement);
}


/**
 * @brief A label statement in Javascript.
 * @example "label:i=1" OR "label:if(){}"
 **/
AST_NODE_HANDLE IOC_CreateLabelledStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN i,
	AST_NODE_HANDLE s
)
{
	// The label name.
	pANTLR3_STRING antlr_string = i->getText(i);
	const char* ioc_char = (char*)antlr_string->chars;

	// Create label statement node.
	ioc::LabelledStatement* pLabelledStatement =
		(ioc::LabelledStatement*)ioc::AstFactory::createAstNode(IocAst_kLabelledStatement);

	// position of this label statement in source code.
	AsignSrcinfoToNode(pLabelledStatement,srcInfo);

	// Store label name as a member variable.
	pLabelledStatement->identifier(ioc_char);
	// Set his child, a statement.
	pLabelledStatement->statement(GetAstNode(s));

	return GetAstHandle(pLabelledStatement);
}


/**
 * @brief `switchStatement` parser rule in grammar.
 * @example "switch(c){case 1: a=1;break; case 2: b=2;break;}"
 **/
AST_NODE_HANDLE IOC_CreateSwitchStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE b
)
{
	ioc::SwitchStatement* pSwitchStatement =
		(ioc::SwitchStatement*)ioc::AstFactory::createAstNode(IocAst_kSwitchStatement);
	AsignSrcinfoToNode(pSwitchStatement,srcInfo);

	pSwitchStatement->expression(GetAstNode(e));
	pSwitchStatement->caseBlock(GetAstNode(b));

	return GetAstHandle(pSwitchStatement);
}


/**
 * @brief `caseBlock` parser rule in grammar.
 * @example "case 1: a=1;break; case 2: b=2;break; default: c=3;break;" in "switch(c){case 1: a=1;break; case 2: b=2;break; default: c=3;break;}"
 **/
AST_NODE_HANDLE IOC_CreateCaseBlock(
	ioc_src_info *srcInfo
) {
	ioc::CaseBlock* pCaseBlock =
		(ioc::CaseBlock*)ioc::AstFactory::createAstNode(IocAst_kCaseBlock);
	AsignSrcinfoToNode(pCaseBlock,srcInfo);

	return GetAstHandle(pCaseBlock);
}


/**
 * @brief `caseBlock` parser rule in grammar.
 * @example "case 1: a=1;break; case 2: b=2;break; default: c=3;break;" in "switch(c){case 1: a=1;break; case 2: b=2;break; default: c=3;break;}"
 **/
AST_NODE_HANDLE IOC_AppendCaseBlock(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE c
) {
	ioc::CaseBlock* pContainer = GetAstNode(self)->AsCaseBlock();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(c));
		return self;
	}
	return NULL;
}



/**
 * @brief `caseClause` parser rule in grammar.
 * @example "case 1: a=1;break;" in "case 1: a=1;break; case 2: b=2;break; default: c=3;break;"
 **/
AST_NODE_HANDLE IOC_CreateCaseClause(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE s
) {
	ioc::CaseClause* pCaseClause =
		(ioc::CaseClause*)ioc::AstFactory::createAstNode(IocAst_kCaseClause);
	AsignSrcinfoToNode(pCaseClause,srcInfo);

	pCaseClause->expression(GetAstNode(e));
	pCaseClause->statement(GetAstNode(s));

	return GetAstHandle(pCaseClause);
}


/**
 * @brief `defaultClause` parser rule in grammar.
 * @example "default: c=3;break;" in "case 1: a=1;break; case 2: b=2;break; default: c=3;break;"
 **/
AST_NODE_HANDLE IOC_CreateDefaultClause(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE s
) {
	ioc::DefaultClause* pDefaultClause =
		(ioc::DefaultClause*)ioc::AstFactory::createAstNode(IocAst_kDefaultClause);
	AsignSrcinfoToNode(pDefaultClause,srcInfo);

	pDefaultClause->statement(GetAstNode(s));

	return GetAstHandle(pDefaultClause);
}


/**
 * @brief `throwStatement` parser rule in grammar.
 * @example "throw a=1;"
 **/
AST_NODE_HANDLE IOC_CreateThrow(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e
) {
	ioc::Throw* pThrow =
		(ioc::Throw*)ioc::AstFactory::createAstNode(IocAst_kThrow);
	AsignSrcinfoToNode(pThrow,srcInfo);

	pThrow->expression(GetAstNode(e));

	return GetAstHandle(pThrow);
}


/**
 * @brief `tryStatement` parser rule in grammar.
 * @example "try{...}catch(err){...}finally{...}"
 **/
AST_NODE_HANDLE IOC_CreateTryStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE s,
	AST_NODE_HANDLE c,
	AST_NODE_HANDLE f
) {
	ioc::TryStatement* pTryStatement =
		(ioc::TryStatement*)ioc::AstFactory::createAstNode(IocAst_kTryStatement);
	AsignSrcinfoToNode(pTryStatement,srcInfo);

	pTryStatement->statement(GetAstNode(s));
	pTryStatement->catchClause(GetAstNode(c));
	pTryStatement->finallyClause(GetAstNode(f));

	return GetAstHandle(pTryStatement);
}


/**
 * @brief `catchClause` parser rule in grammar.
 * @example "catch{...}" in "try{...}catch(err){...}finally{...}"
 **/
AST_NODE_HANDLE IOC_CreateTryCatchStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	pANTLR3_COMMON_TOKEN i,
	AST_NODE_HANDLE s
) {
	// Create parent node.
	ioc::TryCatchStatement* pTryCatchStatement =
		(ioc::TryCatchStatement*)ioc::AstFactory::createAstNode(IocAst_kTryCatchStatement);
	AsignSrcinfoToNode(pTryCatchStatement,srcInfo);

	// Create a child containing Identifier.
	ioc::VariableProxy* pVariableProxy =
		(ioc::VariableProxy*)ioc::AstFactory::createAstNode(IocAst_kVariableProxy);
	//@FIXME : we need assign source info from i token here.
	AsignSrcinfoToNode(pVariableProxy,srcInfo);

	pANTLR3_STRING str = i->getText(i);
	pVariableProxy->js_identifier((char*)str->chars);

	pTryCatchStatement->init(GetAstNode(pVariableProxy));
	pTryCatchStatement->statement(GetAstNode(s));

	return GetAstHandle(pTryCatchStatement);
}


/**
 * @brief `finallyClause` parser rule in grammar.
 * @example "finally{...}" in "try{...}catch(err){...}finally{...}"
 **/
AST_NODE_HANDLE IOC_CreateTryFinallyStatement(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE s
) {
	ioc::TryFinallyStatement* pTryFinallyStatement =
		(ioc::TryFinallyStatement*)ioc::AstFactory::createAstNode(IocAst_kTryFinallyStatement);
	AsignSrcinfoToNode(pTryFinallyStatement,srcInfo);

	pTryFinallyStatement->statement(GetAstNode(s));

	return GetAstHandle(pTryFinallyStatement);
}


AST_NODE_HANDLE IOC_CreateBinaryOperation(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN operatorToken,
	AST_NODE_HANDLE left,
	AST_NODE_HANDLE right
) {
	ioc::BinaryOperation* pOperation =
		(ioc::BinaryOperation*)ioc::AstFactory::createAstNode(IocAst_kBinaryOperation);
	AsignSrcinfoToNode(pOperation,srcInfo);

	pOperation->left(GetAstNode(left));
	pOperation->right(GetAstNode(right));

	pANTLR3_STRING str = operatorToken->getText(operatorToken);
	if(str && str->len){
		const char* ioc_operation = (char*)(str->chars);
		pOperation->opName(ioc_operation);
		// Binary operator.
	}
	return GetAstHandle(pOperation);
}


/**
 * @brief Initial a variable.
 * @example "a=1"
 **/
AST_NODE_HANDLE IOC_CreateAssignment(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN op,
	AST_NODE_HANDLE left,
	AST_NODE_HANDLE right
)
{
	ioc::Assignment* pAssignment =
		(ioc::Assignment*)ioc::AstFactory::createAstNode(IocAst_kAssignment);
	AsignSrcinfoToNode(pAssignment,srcInfo);

	pAssignment->left(GetAstNode(left));
	pAssignment->right(GetAstNode(right));

	pANTLR3_STRING str = op->getText(op);
	if(str && str->len){
		const char* ioc_operation = (char*)(str->chars);
		pAssignment->opName(ioc_operation);
	}
	return GetAstHandle(pAssignment);
}

/**
 * @brief `newExpression` parser rule in grammar.
 * @example "new Object()"
 **/
AST_NODE_HANDLE IOC_CreateNewExpression(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE arg
) {
	ioc::NewExpression* pNewExpression =
		(ioc::NewExpression*)ioc::AstFactory::createAstNode(IocAst_kNewExpression);
	AsignSrcinfoToNode(pNewExpression,srcInfo);

	pNewExpression->expression(GetAstNode(e));
	pNewExpression->argument(GetAstNode(arg));

	return GetAstHandle(pNewExpression);
}


/**
 * @brief Multi property accessor, split by "."
 * @example "a.b.c.fn().d.e.f.fn2().g"
 **/
AST_NODE_HANDLE IOC_CreateMultiPropertyAccessor(
	ioc_src_info *srcInfo
) {
	ioc::MultiPropertyAccessor* pMultiPropertyAccessor =
		(ioc::MultiPropertyAccessor*)ioc::AstFactory::createAstNode(IocAst_kMultiPropertyAccessor);
	AsignSrcinfoToNode(pMultiPropertyAccessor,srcInfo);

	return GetAstHandle(pMultiPropertyAccessor);
}


AST_NODE_HANDLE IOC_AppendMultiPropertyAccessor(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE e
) {
	ioc::MultiPropertyAccessor* pContainer = GetAstNode(self)->AsMultiPropertyAccessor();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(e));
		return self;
	}
	return NULL;
}

/**
 * @brief `callExpression` parser rule in grammar.
 * @example "Object()" in "new Object()" OR "fun(a,b,c)"
 **/
AST_NODE_HANDLE IOC_CreateCall(
	ioc_src_info *srcInfo,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE a
) {
	ioc::Call* pCall =
		(ioc::Call*)ioc::AstFactory::createAstNode(IocAst_kCall);
	AsignSrcinfoToNode(pCall,srcInfo);

	if(ioc::VariableProxy* pVariableProxy = GetAstNode(e)->AsVariableProxy())
	{
		// Function name is not expression.
		pCall->name(pVariableProxy->js_identifier());
		pCall->expression(NULL);
	}
	else
	{
		pCall->name("");
		pCall->expression(GetAstNode(e));
	}

	pCall->arguments(GetAstNode(a));

	return GetAstHandle(pCall);
}

/**
 * @brief `arguments` parser rule in grammar.
 * @example "dateString" in "new Date(dateString)"
 **/
AST_NODE_HANDLE IOC_CreateArguments(
	ioc_src_info *srcInfo
) {
	ioc::Arguments* pArguments =
		(ioc::Arguments*)ioc::AstFactory::createAstNode(IocAst_kArguments);
	AsignSrcinfoToNode(pArguments,srcInfo);

	return GetAstHandle(pArguments);
}

/**
 * @brief `arguments` parser rule in grammar.
 * @example "dateString" in "new Date(dateString)"
 **/
AST_NODE_HANDLE IOC_AppendArguments(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE e
)
{
	ioc::Arguments* pContainer = GetAstNode(self)->AsArguments();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(e));
		return self;
	}
	return NULL;
}


/**
 * @brief `indexSuffix` parser rule in grammar.
 * @example "[i]" in "a[i]"
 **/
AST_NODE_HANDLE IOC_CreateIndexSuffix(
	ioc_src_info *srcInfo,
	AST_NODE_HANDLE e
) {
	ioc::IndexSuffix* pIndexSuffix =
		(ioc::IndexSuffix*)ioc::AstFactory::createAstNode(IocAst_kIndexSuffix);
	AsignSrcinfoToNode(pIndexSuffix,srcInfo);

	pIndexSuffix->expression(GetAstNode(e));

	return GetAstHandle(pIndexSuffix);
}


/**
 * @brief `propertyReferenceSuffix` parser rule in grammar.
 * @example ".property" in "Object.property"
 **/
AST_NODE_HANDLE IOC_CreatePropertyReferenceSuffix(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN identifier
)
{
	ioc::PropertyReferenceSuffix* pPropertyReferenceSuffix =
		(ioc::PropertyReferenceSuffix*)ioc::AstFactory::createAstNode(IocAst_kPropertyReferenceSuffix);
	AsignSrcinfoToNode(pPropertyReferenceSuffix,srcInfo);

	pANTLR3_STRING str = identifier->getText(identifier);
	pPropertyReferenceSuffix->name((char*)str->chars);

	return GetAstHandle(pPropertyReferenceSuffix);
}

AST_NODE_HANDLE IOC_CreateConditional(
	ioc_src_info *srcInfo,
	AST_NODE_HANDLE e,
	AST_NODE_HANDLE e1,
	AST_NODE_HANDLE e2
){
	// Create parent node.
	ioc::Conditional* pConditional =
		(ioc::Conditional*)ioc::AstFactory::createAstNode(IocAst_kConditional);
	AsignSrcinfoToNode(pConditional,srcInfo);

	pConditional->condition(GetAstNode(e));
	pConditional->valueIfTrue(GetAstNode(e1));
	pConditional->valueIfFalse(GetAstNode(e2));

	return GetAstHandle(pConditional);
}

AST_NODE_HANDLE IOC_CreateCompareOperation(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN operatorToken,
	AST_NODE_HANDLE left,
	AST_NODE_HANDLE right
) {
	ioc::CompareOperation* pOperation =
		(ioc::CompareOperation*)ioc::AstFactory::createAstNode(IocAst_kCompareOperation);
	AsignSrcinfoToNode(pOperation,srcInfo);

	pOperation->left(GetAstNode(left));
	pOperation->right(GetAstNode(right));
	pANTLR3_STRING str = operatorToken->getText(operatorToken);
	if(str && str->len){
		char* ioc_operation = (char*)(str->chars);
		pOperation->opName(ioc_operation);
	}
	return GetAstHandle(pOperation);
}

/**
 * @brief `unaryExpression` and `postfixExpression` parser rule in grammar.
 * @example "++i" OR "i++"
 **/
AST_NODE_HANDLE IOC_CreateUnaryOperation(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN token,
	ANTLR3_BOOLEAN isFrontOp,
	AST_NODE_HANDLE e
) {
	pANTLR3_STRING antlr_string = token->getText(token);

	ioc::UnaryOperation* pUnaryOperation =
		(ioc::UnaryOperation*)ioc::AstFactory::createAstNode(IocAst_kUnaryOperation);
	AsignSrcinfoToNode(pUnaryOperation,srcInfo);

	pUnaryOperation->expression(GetAstNode(e));
	pUnaryOperation->isFrontOp(isFrontOp == ANTLR3_TRUE);

	// Save the op name as a string, only for debug.
	pUnaryOperation->opName((char*)antlr_string->chars);
	return GetAstHandle(pUnaryOperation);
}

/// TODO This function is used until PHP frontend support `m_hasVar` member.
AST_NODE_HANDLE IOC_CreateVariableProxy(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN identifier
) {
	ioc::VariableProxy* pVariableProxy =
		(ioc::VariableProxy*)ioc::AstFactory::createAstNode(IocAst_kVariableProxy);
	AsignSrcinfoToNode(pVariableProxy,srcInfo);

	pVariableProxy->js_identifier((char*)identifier->getText(identifier)->chars);

	return GetAstHandle(pVariableProxy);
}

/**
 * @brief `arrayLiteral` parser rule in grammar.
 * @example "[a,b,c]"
 **/
AST_NODE_HANDLE IOC_CreateArrayLiteral(
	ioc_src_info *srcInfo
) {
	ioc::ArrayLiteral* pArrayLiteral =
		(ioc::ArrayLiteral*)ioc::AstFactory::createAstNode(IocAst_kArrayLiteral);
	AsignSrcinfoToNode(pArrayLiteral,srcInfo);

	return GetAstHandle(pArrayLiteral);
}

AST_NODE_HANDLE IOC_AppendArrayLiteral(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE e
) {
	ioc::ArrayLiteral* pContainer = GetAstNode(self)->AsArrayLiteral();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(e));
		return self;
	}

	return NULL;
}

/**
 * @brief `objectLiteral` parser rule in grammar.
 * @example "{a:1,b:2,c:3}"
 **/
AST_NODE_HANDLE IOC_CreateObjectLiteral(
	ioc_src_info *srcInfo
) {
	ioc::ObjectLiteral* pObjectLiteral =
		(ioc::ObjectLiteral*)ioc::AstFactory::createAstNode(IocAst_kObjectLiteral);
	AsignSrcinfoToNode(pObjectLiteral,srcInfo);

	return GetAstHandle(pObjectLiteral);
}

AST_NODE_HANDLE IOC_AppendObjectLiteral(
	AST_NODE_HANDLE self,
	AST_NODE_HANDLE e
)
{
	ioc::ObjectLiteral* pContainer = GetAstNode(self)->AsObjectLiteral();
	if(pContainer)
	{
		pContainer->push_back(GetAstNode(e));

		return self;
	}
	return NULL;
}

/**
 * @brief `propertyNameAndValue` parser rule in grammar.
 * @example "a:1" in "{a:1,b:2,c:3}"
 **/
AST_NODE_HANDLE IOC_CreatePropertyNameAndValue(
	ioc_src_info *srcInfo,
	AST_NODE_HANDLE name,
	AST_NODE_HANDLE e
) {
	// Create parent node.
	ioc::PropertyNameAndValue* pPropertyNameAndValue =
		(ioc::PropertyNameAndValue*)ioc::AstFactory::createAstNode(IocAst_kPropertyNameAndValue);
	AsignSrcinfoToNode(pPropertyNameAndValue,srcInfo);

	pPropertyNameAndValue->name(GetAstNode(name));
	pPropertyNameAndValue->expression(GetAstNode(e));

	return GetAstHandle(pPropertyNameAndValue);
}

AST_NODE_HANDLE IOC_CreateStringLiteral(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN str
) {
	ioc::StringLiteral* pStringLiteral =
		(ioc::StringLiteral*)ioc::AstFactory::createAstNode(IocAst_kStringLiteral);
	AsignSrcinfoToNode(pStringLiteral,srcInfo);

	// Token is not empty
	if (str) {
		pANTLR3_STRING antlr_string = str->getText(str);
		//@FIXME: shall we need process escape here?
		pStringLiteral->str((char*)antlr_string->chars);
	// Token is empty.
	} else {
		// ...
	}

	return GetAstHandle(pStringLiteral);
}

AST_NODE_HANDLE IOC_CreateNumberLiteral(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN num
)
{
	ioc::NumberLiteral* pNumberLiteral =
		(ioc::NumberLiteral*)ioc::AstFactory::createAstNode(IocAst_kNumberLiteral);
	AsignSrcinfoToNode(pNumberLiteral,srcInfo);
	if (num) {
		pANTLR3_STRING antlr_string = num->getText(num);
		pNumberLiteral->num((char*)antlr_string->chars);
	}

	return GetAstHandle(pNumberLiteral);
}

/**
 * @brief A node for null value.
 * @example var a=null;
 **/
AST_NODE_HANDLE IOC_CreateNullLiteral(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN str
) {
	ioc::NullLiteral* pNullLiteral =
		(ioc::NullLiteral*)ioc::AstFactory::createAstNode(IocAst_kNullLiteral);
	AsignSrcinfoToNode(pNullLiteral,srcInfo);

	if (str) {
		pANTLR3_STRING antlr_string = str->getText(str);
		pNullLiteral->value((char*)antlr_string->chars);
	}

	return GetAstHandle(pNullLiteral);
}

/**
 * @brief A node for bool value.
 * @example var a=true;
 **/
AST_NODE_HANDLE IOC_CreateBooleanLiteral(
	ioc_src_info *srcInfo,
	pANTLR3_COMMON_TOKEN str
) {
	ioc::BooleanLiteral* pBooleanLiteral =
		(ioc::BooleanLiteral*)ioc::AstFactory::createAstNode(IocAst_kBooleanLiteral);
	AsignSrcinfoToNode(pBooleanLiteral,srcInfo);

	if (str) {
		pANTLR3_STRING antlr_string = str->getText(str);
		char* value_char = (char*)antlr_string->chars;
		if(STREQ(value_char, "true")) {
			pBooleanLiteral->setValue(true);
		} else {
			pBooleanLiteral->setValue(false);
		}
	}

	return GetAstHandle(pBooleanLiteral);
}


#ifdef __cplusplus
}
#endif
