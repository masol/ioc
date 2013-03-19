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
 
#ifndef  IOC_FRONTEND_ASTTYPES_H
#define  IOC_FRONTEND_ASTTYPES_H


/**
 * @file        asttypes.h
 * @brief       维护AST的类型相关宏。
 **/
 
/// @brief AST中的节点类型。这里的节点只包含可实例化的节点类型，诸如Statement之类的抽象节点类型这里没有列出。
#define STATEMENT_NODE_LIST(V)                    \
	V(SourceElements)                             \
	V(EmptyStatement)                             \
	V(IfStatement)                                \
	V(ContinueStatement)                          \
	V(BreakStatement)                             \
	V(ReturnStatement)                            \
	V(WithStatement)                              \
	V(SwitchStatement)                            \
	V(DoWhileStatement)                           \
	V(WhileStatement)                             \
	V(ForStatement)                               \
	V(ForInStatement)                             \
	V(ForEachStatement)                           \
	V(TryStatement)                               \
	V(TryCatchStatement)                          \
	V(TryFinallyStatement)                        \
	V(FunctionDeclaration)                        \
	V(LabelledStatement)                          \
	V(CaseBlock)                                  \
	V(CaseClause)                                 \
	V(DefaultClause)                              


#define EXPRESSION_NODE_LIST(V)                   \
	V(FunctionExpression)                         \
	V(BinaryOperation)                            \
	V(Assignment)                                 \
	V(UnaryOperation)                             \
	V(VariableDeclarationList)                    \
	V(FormalParameterList)                        \
	V(VariableProxy)                              \
	V(CharacterLiteral) 	                          \
	V(StringLiteral)                              \
	V(NumberLiteral)                              \
	V(IntNumberLiteral)                           \
	V(DoubleNumberLiteral)                        \
	V(BooleanLiteral)                             \
	V(NullLiteral)                                \
	V(ObjectLiteral)                              \
	V(ArrayLiteral)                               \
	V(PropertyNameAndValue)                       \
	V(NewExpression)                              \
	V(MemberExpression)                           \
	V(Arguments)                                  \
	V(IndexSuffix)                                \
	V(PropertyReferenceSuffix)                    \
	V(PropertyAccessor)                           \
	V(MultiPropertyAccessor)                      \
	V(Call)                                       \
	V(Conditional)                                \
	V(Throw)                                      \
	V(CompareOperation)                           \
	V(Map)                                        

		
#define AST_NODE_LIST(V)                          \
	STATEMENT_NODE_LIST(V)                        \
	EXPRESSION_NODE_LIST(V)

/*我们在c头文件中可以引用这些类型。*/
#ifdef __cplusplus
extern "C" {
#endif

#define DECLARE_TYPE_ENUM(type) IocAst_k##type,
typedef	enum	_tagIOCASTTYPES{
	IocAst_kInvalid,
	AST_NODE_LIST(DECLARE_TYPE_ENUM)
	IocAst_kMaxType,
	IocAst_kStatement = -1,
	IocAst_kExpression = -2
}IOCASTTYPES;
#undef  DECLARE_TYPE_ENUM


#ifdef __cplusplus
}
#endif



#endif //IOC_FRONTEND_ASTTYPES_H
 