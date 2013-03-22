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

grammar javascript;

options
{
// Generate C language lexer and parser.
	language=C;
	output=AST;
	ASTLabelType=pANTLR3_BASE_TREE;	// For C target
	backtrack=true;
	memoize=true;
}

//
// Fill in javascriptLexer.*
//

@lexer::includes
{
	#include "frontend/antlrhelper.h"
}

@lexer::members
{
}

//
// Fill in javascriptParser.*
//

// This is what the grammar programmer asked us to put at the top of every file.
@parser::header
{
}

@parser::includes
{
	#include "frontend/antlrhelper.h"
}

@parser::members
{
}

/* ========================================================================= */


// Global variable .


program returns [AST_NODE_HANDLE ret]
@init {
	$ret = NULL;
	sourceElements_rets.ret = NULL;
}
	:
	LineSeparator!* sourceElements_rets=sourceElements LineSeparator!* EOF!
	{
		$ret = sourceElements_rets.ret;
	}
	;
	
sourceElements returns [AST_NODE_HANDLE ret]
@declarations {
	ioc_src_info	ioc_srcinfo;
}
@init {
	$ret = NULL;
	ioc_se.ret = NULL;
	IOC_INIT_SRC_INFO(ioc_srcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($sourceElements.start,ioc_srcinfo);
		/// @brief Create node w/o child.
		$ret = IOC_CreateSourceElements(&ioc_srcinfo, SOURCE_NAME(ISTREAM));
	}
	(
		(
			(
				ioc_se=sourceElement
				{
					$ret = IOC_AppendSourceElements($ret, ioc_se.ret);
				}
			)
			(
				LineSeparator!* ( ioc_se=sourceElement {
					$ret = IOC_AppendSourceElements($ret, ioc_se.ret);
				})
			)*
		)
		|
		// Allow empty file.
	)
	;
	
sourceElement returns [AST_NODE_HANDLE ret]
@init {
	$ret = NULL;
}
@after {
}
	:
	(
		(
			functionDeclaration
			{
				$ret = $functionDeclaration.ret;
			}
		)
		|
		(
			statement
			{
				$ret = $statement.ret;
			}
		)
	)
	;


// functions
functionDeclaration returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	ioc_srcinfo;
}
@init {
$ret = NULL;
ioc_pl.ret = NULL;
ioc_fb.ret = NULL;
IOC_INIT_SRC_INFO(ioc_srcinfo);
}
	:
	'function' LineSeparator!* name=Identifier LineSeparator!* ioc_pl=formalParameterList LineSeparator!* ioc_fb=functionBody
	{
		IOC_ASSIGN_SRC_INFO($functionDeclaration.start,ioc_srcinfo);
		$ret = IOC_CreateFunctionDeclaration(&ioc_srcinfo, name, ioc_pl.ret, ioc_fb.ret);
	}
	;
	
functionExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	ioc_srcinfo;
}
@init {
$ret = NULL;
ioc_pl.ret = NULL;
ioc_fb.ret = NULL;
IOC_INIT_SRC_INFO(ioc_srcinfo);
}
	:
	'function' LineSeparator!* nameToken=Identifier? LineSeparator!* ioc_pl=formalParameterList LineSeparator!* ioc_fb=functionBody
	{
		IOC_ASSIGN_SRC_INFO($functionExpression.start,ioc_srcinfo);
		$ret = IOC_CreateFunctionExpression(&ioc_srcinfo, nameToken, ioc_pl.ret, ioc_fb.ret);
	}
	;
	
formalParameterList returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($formalParameterList.start,iocSrcinfo);
		/// @brief Create node w/o child.
		$ret = IOC_CreateFormalParameterList(&iocSrcinfo);
	} 
	'('
	(
		LineSeparator!* (varToken=Identifier{
			IOC_ASSIGN_SRC_INFO(varToken,iocSrcinfo);
			$ret = IOC_AppendFormalParameterList(&iocSrcinfo,$ret,varToken);
		})
		(
			LineSeparator!* ',' LineSeparator!* (varToken=Identifier{
				IOC_ASSIGN_SRC_INFO(varToken,iocSrcinfo);
				$ret = IOC_AppendFormalParameterList(&iocSrcinfo,$ret, varToken);
			})
		)*
	)?
	LineSeparator!* ')'
	;

functionBody returns [AST_NODE_HANDLE ret]
@init {
$ret = NULL;
ioc_se.ret = NULL;
}
	:
	'{' LineSeparator!* ioc_se=sourceElements LineSeparator!* '}'
	{
		$ret = ioc_se.ret;
	}
	;

// statements
statement returns [AST_NODE_HANDLE ret]
@init {
	$ret = NULL;
}
@after {
}
	:
	(
		( statementBlock {
			$ret = $statementBlock.ret;
		})
		|
		( variableStatement{
			$ret = $variableStatement.ret;
		})
		|
		( emptyStatement{
			$ret = $emptyStatement.ret;
		})
		|
		( expressionStatement {
			$ret = $expressionStatement.ret;
		})
		|
		( ifStatement{
			$ret = $ifStatement.ret;
		})
		|( iterationStatement{
			$ret = $iterationStatement.ret;
		})
		| ( continueStatement{
			$ret = $continueStatement.ret;
		})
		| (breakStatement{
			$ret = $breakStatement.ret;
		})
		| (returnStatement{
			$ret = $returnStatement.ret;
		})
		| (withStatement{
			$ret = $withStatement.ret;
		})
		| (labelledStatement{
			$ret = $labelledStatement.ret;
		})
		| (switchStatement{
			$ret = $switchStatement.ret;
		})
		| (throwStatement{
			$ret = $throwStatement.ret;
		})
		| (tryStatement{
			$ret = $tryStatement.ret;
		})
	)
	;

statementBlock returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_stmts.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	'{' LineSeparator!* ioc_stmts=statementList? LineSeparator!* '}'
	{
		if(ioc_stmts.ret) {
			$ret = ioc_stmts.ret;
		} else {
			IOC_ASSIGN_SRC_INFO($statementBlock.start,iocSrcinfo);
			/// @brief There's nothing in braces, create node w/o child.
			$ret = IOC_CreateSourceElements(&iocSrcinfo, SOURCE_NAME(ISTREAM));
		}
	}
	;

statementList returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_stmt.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($statementList.start,iocSrcinfo);
		/// @brief Create node w/o child.
		$ret = IOC_CreateSourceElements(&iocSrcinfo, SOURCE_NAME(ISTREAM));
	}
	(ioc_stmt=statement{
		$ret = IOC_AppendSourceElements($ret, ioc_stmt.ret);
	})
	(
		LineSeparator!* (ioc_stmt=statement{
			$ret = IOC_AppendSourceElements($ret, ioc_stmt.ret);
		})
	)*
	;
	
variableStatement returns [AST_NODE_HANDLE ret]
@init {
$ret = NULL;
ioc_vdl.ret = NULL;
}
	:
	vartoken='var'^ LineSeparator!* (ioc_vdl=variableDeclarationList{
		$ret = ioc_vdl.ret;
	}) (LineSeparator | ';')!
	;
	
variableDeclarationList returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_vd.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($variableDeclarationList.start,iocSrcinfo);
		$ret = IOC_CreateVariableDeclarationList(&iocSrcinfo);
	}
	(ioc_vd=variableDeclaration{
		$ret = IOC_AppendVariableDeclarationList($ret, ioc_vd.ret);
	})
	(
		LineSeparator!* token=',' LineSeparator!* (ioc_vd=variableDeclaration{
			$ret = IOC_AppendVariableDeclarationList($ret, ioc_vd.ret);
		})
	)*
	;
	
variableDeclarationListNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_vd.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($variableDeclarationListNoIn.start,iocSrcinfo);
		$ret = IOC_CreateVariableDeclarationList(&iocSrcinfo);
	}
	(ioc_vd=variableDeclarationNoIn{
		$ret = IOC_AppendVariableDeclarationList($ret, ioc_vd.ret);
	})
	(
		LineSeparator!* token=',' LineSeparator!* (ioc_vd=variableDeclarationNoIn{
			$ret = IOC_AppendVariableDeclarationList($ret, ioc_vd.ret);
		})
	)*
	;
	
variableDeclaration returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_i.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(token=Identifier^ LineSeparator!* ioc_i=initialiser? {
		IOC_ASSIGN_SRC_INFO($variableDeclaration.start,iocSrcinfo);
		if(ioc_i.ret) {
			$ret = IOC_CreateVariableDeclaration_WithVar(&iocSrcinfo, token, ioc_i.ret);
		} else {
			$ret = IOC_CreateVariableProxy_WithVar(&iocSrcinfo, token); // variable not initial.
		}
	})
	;
	
variableDeclarationNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_i.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(token=Identifier^ LineSeparator!* ioc_i=initialiserNoIn? {
		IOC_ASSIGN_SRC_INFO($variableDeclarationNoIn.start,iocSrcinfo);
		if(ioc_i.ret) {
			$ret = IOC_CreateVariableDeclaration_WithVar(&iocSrcinfo, token, ioc_i.ret);
		} else {
			$ret = IOC_CreateVariableProxy_WithVar(&iocSrcinfo, token); // variable not initial.
		}
	})
	;
	
initialiser returns [AST_NODE_HANDLE ret]
	:
	{
		$ret = NULL;
		ioc_ae.ret = NULL;
	}
	token='=' LineSeparator!* ioc_ae=assignmentExpression
	{
		$ret = ioc_ae.ret;
	}
	;
	
initialiserNoIn returns [AST_NODE_HANDLE ret]
	:
	{
		$ret = NULL;
		ioc_ae.ret = NULL;
	}
	token='=' LineSeparator!* ioc_ae=assignmentExpressionNoIn
	{
		$ret = ioc_ae.ret;
	}
	;
	
emptyStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}	:
	token=';'
	{
		IOC_ASSIGN_SRC_INFO($emptyStatement.start,iocSrcinfo);
		$ret = IOC_CreateEmptyStatement(&iocSrcinfo, token);
	}
	;
	
expressionStatement returns [AST_NODE_HANDLE ret]
@init
{
$ret = NULL;
ioc_e.ret = NULL;
}
	:
	(
		ioc_e=expression
		{
			$ret = ioc_e.ret;
		}
	)
	(LineSeparator | ';')!
	;
	
ifStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_e.ret = NULL;
ioc_s1.ret = NULL;
ioc_s2.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}	:
	ifToken='if' LineSeparator!* '(' LineSeparator!* ioc_e=expression LineSeparator!* ')' LineSeparator!* ioc_s1=statement (LineSeparator!* 'else' LineSeparator!* ioc_s2=statement)?
	{
		IOC_ASSIGN_SRC_INFO($ifStatement.start,iocSrcinfo);
		$ret = IOC_CreateIfStatement(&iocSrcinfo, ifToken, ioc_e.ret, ioc_s1.ret, ioc_s2.ret);
	}
	;
	
iterationStatement returns [AST_NODE_HANDLE ret]
	:
	{
		$ret = NULL;
		r1.ret = NULL;
		r2.ret = NULL;
		r3.ret = NULL;
		r4.ret = NULL;
	}
	r1=doWhileStatement{
		$ret = r1.ret;
	}
	|
	r2=whileStatement{
		$ret = r2.ret;
	}
	|
	r3=forStatement{
		$ret = r3.ret;
	}
	|
	r4=forInStatement{
		$ret = r4.ret;
	}
	;
	
doWhileStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_s.ret = NULL;
ioc_e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}	:
	doToken='do' LineSeparator!* ioc_s=statement LineSeparator!* whileToken='while' LineSeparator!* '(' ioc_e=expression ')' (LineSeparator | ';')!
	{
		IOC_ASSIGN_SRC_INFO($doWhileStatement.start,iocSrcinfo);
		$ret = IOC_CreateDoWhileStatement(&iocSrcinfo, doToken, ioc_s.ret, whileToken, ioc_e.ret);
	}
	;
	
whileStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_e.ret = NULL;
ioc_s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	whileToken='while' LineSeparator!* '(' LineSeparator!* ioc_e=expression LineSeparator!* ')' LineSeparator!* ioc_s=statement
	{
		IOC_ASSIGN_SRC_INFO($whileStatement.start,iocSrcinfo);
		$ret = IOC_CreateWhileStatement(&iocSrcinfo, whileToken, ioc_e.ret, ioc_s.ret);
	}
	;
	
forStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_fsip.ret = NULL;
ioc_e1.ret = NULL;
ioc_e2.ret = NULL;
ioc_s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{

	}
	forToken='for' LineSeparator!* '(' (LineSeparator!* ioc_fsip=forStatementInitialiserPart)? LineSeparator!* ';' (LineSeparator!* ioc_e1=expression)? LineSeparator!* ';' (LineSeparator!* ioc_e2=expression)? LineSeparator!* ')' LineSeparator!* ioc_s=statement
	{
		IOC_ASSIGN_SRC_INFO($forStatement.start,iocSrcinfo);
		$ret = IOC_CreateForStatement(&iocSrcinfo, forToken, ioc_fsip.ret, ioc_e1.ret, ioc_e2.ret, ioc_s.ret);
	}
	;
	
forStatementInitialiserPart returns [AST_NODE_HANDLE ret]
	:
	{
		$ret = NULL;
		e.ret = NULL;
		v.ret = NULL;
	}
	(e=expressionNoIn{
		$ret = e.ret;
	})
	|
	(token='var' LineSeparator!* v=variableDeclarationListNoIn{
		$ret = v.ret;
	})
	;
	
forInStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
i.ret = NULL;
e.ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	forToken='for' LineSeparator!* '(' LineSeparator!* i=forInStatementInitialiserPart LineSeparator!* inToken='in' LineSeparator!* e=expression LineSeparator!* ')' LineSeparator!* s=statement
	{
		IOC_ASSIGN_SRC_INFO($forInStatement.start,iocSrcinfo);
		$ret = IOC_CreateForInStatement(&iocSrcinfo, forToken, i.ret, inToken, e.ret, s.ret);
	}
	;
	
forInStatementInitialiserPart returns [AST_NODE_HANDLE ret]
	:
	{
		$ret = NULL;
		l.ret = NULL;
		v.ret = NULL;
	}
	(l=leftHandSideExpression{
		$ret = l.ret;
	})
	|
	(token='var' LineSeparator!* v=variableDeclarationNoIn{
		$ret = v.ret;
	})
	;

continueStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='continue' i=Identifier? (LineSeparator | ';')!
	{
		IOC_ASSIGN_SRC_INFO($continueStatement.start,iocSrcinfo);
		$ret = IOC_CreateContinueStatement(&iocSrcinfo, token, i);
	}
	;

breakStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='break' i=Identifier? (LineSeparator | ';')!
	{
		IOC_ASSIGN_SRC_INFO($breakStatement.start,iocSrcinfo);
		if(i) {
			$ret = IOC_CreateBreakStatement(&iocSrcinfo, token, i);
		} else {
			$ret = IOC_CreateBreakStatement(&iocSrcinfo, token, NULL);
		}
	}
	;

returnStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='return' e=expression? (LineSeparator | ';')!
	{
		IOC_ASSIGN_SRC_INFO($returnStatement.start,iocSrcinfo);
		$ret = IOC_CreateReturnStatement(&iocSrcinfo, token, e.ret);
	}
	;
	
withStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='with' LineSeparator!* '(' LineSeparator!* e=expression LineSeparator!* ')' LineSeparator!* s=statement
	{
		IOC_ASSIGN_SRC_INFO($withStatement.start,iocSrcinfo);
		$ret = IOC_CreateWithStatement(&iocSrcinfo, token, e.ret, s.ret);
	}
	;

labelledStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	i=Identifier LineSeparator!* ':' LineSeparator!* s=statement
	{
		IOC_ASSIGN_SRC_INFO($labelledStatement.start,iocSrcinfo);
		$ret = IOC_CreateLabelledStatement(&iocSrcinfo, i, s.ret);
	}
	;
	
switchStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
b.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='switch' LineSeparator!* '(' LineSeparator!* e=expression LineSeparator!* ')' LineSeparator!* b=caseBlock
	{
		IOC_ASSIGN_SRC_INFO($switchStatement.start,iocSrcinfo);
		$ret = IOC_CreateSwitchStatement(&iocSrcinfo, token, e.ret, b.ret);
	}
	;
	
caseBlock returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
c.ret = NULL;
d.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($caseBlock.start,iocSrcinfo);
		$ret = IOC_CreateCaseBlock(&iocSrcinfo);
	}
	'{' (LineSeparator!* (c=caseClause{
		$ret = IOC_AppendCaseBlock($ret, c.ret);
	}))* (LineSeparator!* (d=defaultClause{
		$ret = IOC_AppendCaseBlock($ret, d.ret);
	}) (LineSeparator!* (c=caseClause{
		$ret = IOC_AppendCaseBlock($ret, c.ret);
	}))*)? LineSeparator!* '}'
	;

caseClause returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
ioc_e.ret = NULL;
ioc_sl.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='case' LineSeparator!* ioc_e=expression LineSeparator!* ':' LineSeparator!* ioc_sl=statementList?
	{
		IOC_ASSIGN_SRC_INFO($caseClause.start,iocSrcinfo);
		$ret = IOC_CreateCaseClause(&iocSrcinfo, token, ioc_e.ret, ioc_sl.ret);
	}
	;
	
defaultClause returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='default' LineSeparator!* ':' LineSeparator!* s=statementList?
	{
		IOC_ASSIGN_SRC_INFO($defaultClause.start,iocSrcinfo);
		$ret = IOC_CreateDefaultClause(&iocSrcinfo, token, s.ret);
	}
	;
	
throwStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='throw' e=expression (LineSeparator | ';')!
	{
		IOC_ASSIGN_SRC_INFO($throwStatement.start,iocSrcinfo);
		$ret = IOC_CreateThrow(&iocSrcinfo, token, e.ret);
	}
	;

tryStatement returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
s.ret = NULL;
c.ret = NULL;
f.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='try' LineSeparator!* s=statementBlock LineSeparator!* (f=finallyClause | c=catchClause (LineSeparator!* (f=finallyClause{
		IOC_ASSIGN_SRC_INFO($tryStatement.start,iocSrcinfo);
		$ret = IOC_CreateTryStatement(&iocSrcinfo, token, s.ret, c.ret, f.ret);
	}))?)
	;
       
catchClause returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='catch' LineSeparator!* '(' LineSeparator!* i=Identifier LineSeparator!* ')' LineSeparator!* s=statementBlock
	{
		IOC_ASSIGN_SRC_INFO($catchClause.start,iocSrcinfo);
		$ret = IOC_CreateTryCatchStatement(&iocSrcinfo, token, i, s.ret);
	}
	;
	
finallyClause returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
s.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	token='finally' LineSeparator!* s=statementBlock
	{
		IOC_ASSIGN_SRC_INFO($finallyClause.start,iocSrcinfo);
		$ret = IOC_CreateTryFinallyStatement(&iocSrcinfo, token, s.ret);
	}
	;

// expressions
expression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
l.ret = NULL;
r.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		l=assignmentExpression
		{
			$ret = l.ret;
		}
	)
	(
		LineSeparator!* token=',' LineSeparator!* (r=assignmentExpression {
			IOC_ASSIGN_SRC_INFO($expression.start,iocSrcinfo);
			// Assume comma is the same as the binary operation `+`.
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, r.ret);
		})
	)*
	;
	
expressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
l.ret = NULL;
r.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	l=assignmentExpressionNoIn
	{
		$ret = l.ret;
	}
	(
		LineSeparator!* token=',' LineSeparator!* (r=assignmentExpressionNoIn{
			IOC_ASSIGN_SRC_INFO($expressionNoIn.start,iocSrcinfo);
			// Assume comma is the same as the binary operation `+`.
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, r.ret);
		})
	)*
	;
	
assignmentExpression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
iocRetVar.ret = NULL;
op.ret = NULL;
iocleft.ret = NULL;
iocright.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		iocRetVar=conditionalExpression
		{
			$ret = iocRetVar.ret;
		}
	)
	|
	(
		iocleft=leftHandSideExpression LineSeparator!* op=assignmentOperator LineSeparator!* iocright=assignmentExpression
		{
			IOC_ASSIGN_SRC_INFO($iocleft.start,iocSrcinfo);
			if(op.ret) {
				$ret = IOC_CreateAssignment(
					&iocSrcinfo, op.ret, iocleft.ret, iocright.ret
				);
			}
		}
	)
	;
	
assignmentExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
ce.ret = NULL;
op.ret = NULL;
iocleft.ret = NULL;
iocright.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(ce=conditionalExpressionNoIn{
		$ret = ce.ret;
	})
	|
	iocleft=leftHandSideExpression LineSeparator!* op=assignmentOperator LineSeparator!* iocright=assignmentExpressionNoIn{
		IOC_ASSIGN_SRC_INFO($iocleft.start,iocSrcinfo);
		if(op.ret) {
			$ret = IOC_CreateAssignment(
				&iocSrcinfo, op.ret, iocleft.ret, iocright.ret
			);
		}
	}
	;
	
leftHandSideExpression returns [AST_NODE_HANDLE ret]
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
}
	:
	(
		r1=callExpression { $ret = r1.ret; }
	)
	|
	(
		r2=newExpression { $ret = r2.ret; }
	)
	;
	
newExpression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		r1 = memberExpression
		{
			$ret = r1.ret;
		}
	)
	|
	(
		token = 'new' LineSeparator!* r2 = newExpression
		{
			IOC_ASSIGN_SRC_INFO($token,iocSrcinfo);
			$ret = IOC_CreateNewExpression(&iocSrcinfo, token, r2.ret,NULL);
		}
	)
	;
	
memberExpression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
AST_NODE_HANDLE ROOT;
}
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
m.ret = NULL;
a.ret = NULL;
ioc_mes.ret = NULL;
ROOT = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		(
			{
				//here we can not get memberExpression.so in AppendMultiPropertyAccessor we reinit the position.
				//IOC_ASSIGN_SRC_INFO(memberExpression.start,iocSrcinfo);
				ROOT = IOC_CreateMultiPropertyAccessor(&iocSrcinfo);
			}
			r1 = primaryExpression
			{
				$ret = r1.ret;
				IOC_AppendMultiPropertyAccessor(ROOT, r1.ret);
			}
		)
		|
		(
			r2=functionExpression
			{
				$ret = r2.ret;
			}
		)
		|
		(
			token='new' LineSeparator!* m=memberExpression LineSeparator!* a=arguments
			{
				IOC_ASSIGN_SRC_INFO($token,iocSrcinfo);
				$ret = IOC_CreateNewExpression(&iocSrcinfo, token, m.ret, a.ret);
			}
		)
	)
	(
		LineSeparator!*
		(
			ioc_mes = memberExpressionSuffix
			{
				IOC_AppendMultiPropertyAccessor(ROOT, ioc_mes.ret);
				$ret = ROOT;
			}
		)
	)*
	;
	
memberExpressionSuffix returns [AST_NODE_HANDLE ret]
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
}
	:
	(
		r1=indexSuffix
		{
			$ret = r1.ret;
		}
	)
	|
	(
		r2=propertyReferenceSuffix
		{
			$ret = r2.ret;
		}
	)
	;

callExpression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
AST_NODE_HANDLE CALL;
AST_NODE_HANDLE MPA; // MultiPropertyAccessor
}
@init
{
$ret = NULL;
m.ret = NULL;
a.ret = NULL;
CALL = NULL;
MPA = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		m = memberExpression
	)
	LineSeparator!*
	(
		a = arguments
		{
			IOC_ASSIGN_SRC_INFO($callExpression.start,iocSrcinfo);

			MPA = IOC_CreateMultiPropertyAccessor(&iocSrcinfo);
			
			CALL = IOC_CreateCall(&iocSrcinfo, m.ret, a.ret); 
			// retval.ret = "fn1()"
			MPA = IOC_AppendMultiPropertyAccessor(MPA, CALL);
			$ret = CALL;
		}
	)
	(
		LineSeparator!*
		(
			call_suffix = callExpressionSuffix
			{
				MPA = IOC_AppendMultiPropertyAccessor(MPA, call_suffix.ret);
				$ret = MPA;
			}
		)
	)*
	(
	)
	;
	
callExpressionSuffix returns [AST_NODE_HANDLE ret]
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
r3.ret = NULL;
}
	:
	( r1=arguments {
		$ret = r1.ret;
	})
	|
	(r2=indexSuffix{
		$ret = r2.ret;
	})
	|
	( r3=propertyReferenceSuffix {
		$ret = r3.ret;
	})
	;

arguments returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
a.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($arguments.start,iocSrcinfo);
		$ret = IOC_CreateArguments(&iocSrcinfo);
	}
	'('
	(
		LineSeparator!*
		(a=assignmentExpression{
			$ret = IOC_AppendArguments($ret, a.ret);
		})
		(
			LineSeparator!* ',' LineSeparator!*
			(a=assignmentExpression{
				$ret = IOC_AppendArguments($ret, a.ret);
			})
		)*
	)?
	LineSeparator!*
	')'
	;
	
indexSuffix returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	'[' LineSeparator!* e=expression LineSeparator!* ']'
	{
		IOC_ASSIGN_SRC_INFO($indexSuffix.start,iocSrcinfo);
		$ret = IOC_CreateIndexSuffix(&iocSrcinfo, e.ret);
	}
	;	
	
propertyReferenceSuffix returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	dot = '.' LineSeparator!*
	(
		token = Identifier
		{
			IOC_ASSIGN_SRC_INFO($propertyReferenceSuffix.start,iocSrcinfo);			
			$ret = IOC_CreatePropertyReferenceSuffix(&iocSrcinfo, token);
		}
	)
	;
	
assignmentOperator returns [pANTLR3_COMMON_TOKEN ret]
	:
	{
		$ret = NULL;
	}
	token=('=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '>>>=' | '&=' | '^=' | '|=')
	{
		$ret = token;
	}
	;

conditionalExpression returns [AST_NODE_HANDLE ret]
@declarations
{
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
e1.ret = NULL;
e2.ret = NULL;
token1 = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	e=logicalORExpression
	(
		LineSeparator!* token1='?' LineSeparator!* e1=assignmentExpression LineSeparator!* token2=':' LineSeparator!* e2=assignmentExpression
	)?
	{
		IOC_ASSIGN_SRC_INFO($conditionalExpression.start,iocSrcinfo);
		if(token1){
			$ret = IOC_CreateConditional(&iocSrcinfo, e.ret, e1.ret, e2.ret);
		}else{
			$ret = e.ret;
		}
	}
	;

conditionalExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e1.ret = NULL;
e2.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	e=logicalORExpressionNoIn
	(
		LineSeparator!* token1='?' LineSeparator!* e1=assignmentExpressionNoIn LineSeparator!* token2=':' LineSeparator!* e2=assignmentExpressionNoIn
	)?
	{
		IOC_ASSIGN_SRC_INFO($conditionalExpressionNoIn.start,iocSrcinfo);
		if(token1){
			$ret = IOC_CreateConditional(&iocSrcinfo, e.ret, e1.ret, e2.ret);
		}else{
			$ret = e.ret;
		}
	}
	;

logicalORExpression  returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=logicalANDExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='||' LineSeparator!* right=logicalANDExpression
		{
			IOC_ASSIGN_SRC_INFO($logicalORExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
logicalORExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=logicalANDExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='||' LineSeparator!* right=logicalANDExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($logicalORExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
logicalANDExpression  returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseORExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='&&' LineSeparator!* right=bitwiseORExpression
		{
			IOC_ASSIGN_SRC_INFO($logicalANDExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
logicalANDExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseORExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='&&' LineSeparator!* right=bitwiseORExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($logicalANDExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseORExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseXORExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='|' LineSeparator!* right=bitwiseXORExpression
		{
			IOC_ASSIGN_SRC_INFO($bitwiseORExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseORExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseXORExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='|' LineSeparator!* right=bitwiseXORExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($bitwiseORExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseXORExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseANDExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='^' LineSeparator!* right=bitwiseANDExpression
		{
			IOC_ASSIGN_SRC_INFO($bitwiseXORExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseXORExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=bitwiseANDExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='^' LineSeparator!* right=bitwiseANDExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($bitwiseXORExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseANDExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=equalityExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='&' LineSeparator!* right=equalityExpression
		{
			IOC_ASSIGN_SRC_INFO($bitwiseANDExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
bitwiseANDExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=equalityExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token='&' LineSeparator!* right=equalityExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($bitwiseANDExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
equalityExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=relationalExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('==' | '!=' | '===' | '!==') LineSeparator!* right=relationalExpression
		{
			IOC_ASSIGN_SRC_INFO($equalityExpression.start,iocSrcinfo);
			$ret = IOC_CreateCompareOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;

equalityExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=relationalExpressionNoIn
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('==' | '!=' | '===' | '!==') LineSeparator!* right=relationalExpressionNoIn
		{
			IOC_ASSIGN_SRC_INFO($equalityExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateCompareOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;
	
relationalExpression   returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=shiftExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('<' | '>' | '<=' | '>=' | 'instanceof' | 'in') LineSeparator!* right=shiftExpression
		{
			IOC_ASSIGN_SRC_INFO($relationalExpression.start,iocSrcinfo);
			$ret = IOC_CreateCompareOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;

relationalExpressionNoIn returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=shiftExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('<' | '>' | '<=' | '>=' | 'instanceof') LineSeparator!* right=shiftExpression
		{
			IOC_ASSIGN_SRC_INFO($relationalExpressionNoIn.start,iocSrcinfo);
			$ret = IOC_CreateCompareOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;

shiftExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=additiveExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('<<' | '>>' | '>>>') LineSeparator!* right=additiveExpression
		{
			IOC_ASSIGN_SRC_INFO($shiftExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;

additiveExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
left.ret = NULL;
right.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	left=multiplicativeExpression
	{
		$ret = left.ret;
	}
	(
		LineSeparator!* token=('+' | '-') LineSeparator!* right=multiplicativeExpression
		{
			IOC_ASSIGN_SRC_INFO($additiveExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, right.ret);
		}
	)*
	;

multiplicativeExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
l.ret = NULL;
r.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	l=unaryExpression
	{
		$ret = l.ret;
	}
	(
		LineSeparator!* token=('*' | '/' | '%') LineSeparator!* r=unaryExpression
		{
			IOC_ASSIGN_SRC_INFO($multiplicativeExpression.start,iocSrcinfo);
			$ret = IOC_CreateBinaryOperation(&iocSrcinfo, token, $ret, r.ret);
		}
	)*
	;

unaryExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init{
$ret = NULL;
e1.ret = NULL;
e2.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(e1=postfixExpression{
		$ret = e1.ret;
	})
	|
	(token=('delete' | 'void' | 'typeof' | '++' | '--' | '+' | '-' | '~' | '!') e2=unaryExpression{
		IOC_ASSIGN_SRC_INFO($token,iocSrcinfo);
		if(token) {
			/// @brief True, operation is in front of expression.
			$ret = IOC_CreateUnaryOperation(&iocSrcinfo, token, ANTLR3_TRUE, e2.ret);
		} else {
			$ret = e2.ret;
		}
	})
	;

postfixExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	e=leftHandSideExpression token=('++' | '--')?
	{
		IOC_ASSIGN_SRC_INFO($postfixExpression.start,iocSrcinfo);
		if(token){
			/// @brief True, operation is at end of expression.
			$ret = IOC_CreateUnaryOperation(&iocSrcinfo, token, ANTLR3_FALSE, e.ret);
		}else{
			$ret = e.ret;
		}
	}
	;

primaryExpression returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init
{
$ret = NULL;
r1.ret = NULL;
r2.ret = NULL;
r3.ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(ioc_token='this'{
		IOC_ASSIGN_SRC_INFO($primaryExpression.start,iocSrcinfo);
		$ret = IOC_CreateVariableProxy(&iocSrcinfo, ioc_token);
	})
	|
	(
		token=Identifier
		{
			IOC_ASSIGN_SRC_INFO($primaryExpression.start,iocSrcinfo);
			$ret = IOC_CreateVariableProxy(&iocSrcinfo, token);
		}
	)
	|
	(r1=literal{ $ret = r1.ret; })
	|
	(r2=arrayLiteral{ $ret = r2.ret; })
	|
	(r3=objectLiteral{ $ret = r3.ret; })
	|
	'(' LineSeparator!* (e=expression{ $ret = e.ret; }) LineSeparator!* ')'
	;
	
// arrayLiteral definition.
arrayLiteral returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($arrayLiteral.start,iocSrcinfo);
		$ret = IOC_CreateArrayLiteral(&iocSrcinfo);
	}
	'[' LineSeparator!* (e=assignmentExpression{
		$ret = IOC_AppendArrayLiteral($ret, e.ret);
	})? (LineSeparator!* ',' (LineSeparator!* (e=assignmentExpression{
		$ret = IOC_AppendArrayLiteral($ret, e.ret);
	}))?)* LineSeparator!* ']'
	;
       
// objectLiteral definition.
objectLiteral returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
p.ret= NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	{
		IOC_ASSIGN_SRC_INFO($objectLiteral.start,iocSrcinfo);
		$ret = IOC_CreateObjectLiteral(&iocSrcinfo);
	}
	'{' LineSeparator!* (p=propertyNameAndValue{
		$ret = IOC_AppendObjectLiteral($ret, p.ret);
	})? (LineSeparator!* ',' (LineSeparator!* (p=propertyNameAndValue{
		$ret = IOC_AppendObjectLiteral($ret, p.ret);
	}))?)* LineSeparator!* '}'
	;
	
propertyNameAndValue returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
p.ret = NULL;
e.ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	p=propertyName LineSeparator!* ':' LineSeparator!* e=assignmentExpression
	{
		IOC_ASSIGN_SRC_INFO($propertyNameAndValue.start,iocSrcinfo);
		$ret = IOC_CreatePropertyNameAndValue(&iocSrcinfo, p.ret, e.ret);
	}
	;

propertyName returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
$ret = NULL;
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		t1=Identifier
		{

			IOC_ASSIGN_SRC_INFO($propertyName.start,iocSrcinfo);
			$ret = IOC_CreateVariableProxy(&iocSrcinfo, t1);
		}
	)
	|
	(
		t2=StringLiteral
		{
			IOC_ASSIGN_SRC_INFO($propertyName.start,iocSrcinfo);
			$ret = IOC_CreateStringLiteral(&iocSrcinfo, t2);
		}
	)
	|
	(
		t3=NumericLiteral
		{
			IOC_ASSIGN_SRC_INFO($propertyName.start,iocSrcinfo);
			$ret = IOC_CreateNumberLiteral(&iocSrcinfo, t3);
		}
	)
	;

// primitive literal definition.
literal returns [AST_NODE_HANDLE ret]
@declarations {
ioc_src_info	iocSrcinfo;
}
@init {
IOC_INIT_SRC_INFO(iocSrcinfo);
}
	:
	(
		token='null'
		{
			IOC_ASSIGN_SRC_INFO($literal.start,iocSrcinfo);
			$ret = IOC_CreateNullLiteral(&iocSrcinfo, token);
		}
	)
	|
	(
		token='true'
		{
			IOC_ASSIGN_SRC_INFO($literal.start,iocSrcinfo);
			$ret = IOC_CreateBooleanLiteral(&iocSrcinfo, token);
		}
	)
	|
	(
		token='false'
		{
			IOC_ASSIGN_SRC_INFO($literal.start,iocSrcinfo);
			$ret = IOC_CreateBooleanLiteral(&iocSrcinfo, token);
		}
	)
	|
	(
		token=StringLiteral
		{
			IOC_ASSIGN_SRC_INFO($literal.start,iocSrcinfo);
			$ret = IOC_CreateStringLiteral(&iocSrcinfo, token);
		}
	)
	|
	(
		token=NumericLiteral
		{
			IOC_ASSIGN_SRC_INFO($literal.start,iocSrcinfo);
			$ret = IOC_CreateNumberLiteral(&iocSrcinfo, token);
		}
	)
	;
	
// lexer rules.

StringLiteral
@declarations
{
	pANTLR3_STRING buf, newLine, tab, str;
}
@init
{
	
	buf = GETTEXT()->factory->newRaw(GETTEXT()->factory);
	newLine = GETTEXT()->factory->newStr8(GETTEXT()->factory, (pANTLR3_UINT8)"\n");
	tab = GETTEXT()->factory->newStr8(GETTEXT()->factory, (pANTLR3_UINT8)"\t");
}
	:
	(
		begin_token = '"'
		{
			buf->addc(buf, begin_token);
		}
		(
			doubleChar = DoubleStringCharacter
			{
				str = $doubleChar->getText($doubleChar);
				
				// Escape char.
				if(!str->compare(str, "\\n"))
				{
					buf->appendS(buf, newLine);
				}
				else if(!str->compare(str, "\\t"))
				{
					buf->appendS(buf, tab);
				}
				// NOT escape char.
				else
				{
					buf->appendS(buf, str);
				}
				
			}
		)*
		end_token = '"'
		{
			buf->addc(buf, end_token);
		}
		{
			SETTEXT(buf);
		}
	)
	|
	(
		'\''
		(
			SingleStringCharacter {}
		)*
		'\''
	)
	;
	
fragment DoubleStringCharacter
	: 
	reg = ~('"' | '\\' | LineSeparator) // Ang single char except these rule.
	|
	( // A escape char
		'\\' {}
		esc = EscapeSequence {}
	)
	;

fragment SingleStringCharacter
	:
	~('\'' | '\\' | LineSeparator)
	|
	(
		'\\' {}
		EscapeSequence {}
	)
	;

fragment EscapeSequence
	:
	(
		esc = CharacterEscapeSequence {}
	)
	|
	'0'
	|
	(
		HexEscapeSequence {}
	)
	|
	(
		UnicodeEscapeSequence {}
	)
	;
	
fragment CharacterEscapeSequence
	:
	(
		esc = SingleEscapeCharacter {}
	)
	|
	(
		NonEscapeCharacter {}
	)
	;

fragment NonEscapeCharacter
	: ~(EscapeCharacter | LineSeparator)
	;

fragment SingleEscapeCharacter
	: '\'' | '"' | '\\' | 'b' | 'f' | 'n' | 'r' | 't' | 'v'
	;

fragment EscapeCharacter
	: SingleEscapeCharacter
	| DecimalDigit
	| 'x'
	| 'u'
	;
	
fragment HexEscapeSequence
	: 'x' HexDigit HexDigit
	;
	
fragment UnicodeEscapeSequence
	: 'u' HexDigit HexDigit HexDigit HexDigit
	;
	
NumericLiteral
	: DecimalLiteral
	| HexIntegerLiteral
	;
	
fragment HexIntegerLiteral
	: '0' ('x' | 'X') HexDigit+
	;
	
fragment HexDigit
	: DecimalDigit | ('a'..'f') | ('A'..'F')
	;
	
fragment DecimalLiteral
	: DecimalDigit+ '.' DecimalDigit* ExponentPart?
	| '.'? DecimalDigit+ ExponentPart?
	;
	
fragment DecimalDigit
	: ('0'..'9')
	;

fragment ExponentPart
	: ('e' | 'E') ('+' | '-') ? DecimalDigit+
	;

Identifier
	: IdentifierStart IdentifierPart*
	;
	
fragment IdentifierStart
	: UnicodeLetter{/*IOC_ANTLR_DEBUG("found UnicodeLetter: \%s", GETTEXT()->chars);*/}
	| '$'
	| '_'
        | '\\' UnicodeEscapeSequence
        ;
        
fragment IdentifierPart
	: (IdentifierStart) => IdentifierStart // Avoids ambiguity, as some IdentifierStart chars also match following alternatives.
	| UnicodeDigit
	| UnicodeConnectorPunctuation
	;
	
fragment UnicodeLetter		// Any character in the Unicode categories "Uppercase letter (Lu)", 
	: '\u0041'..'\u005A'	// "Lowercase letter (Ll)", "Titlecase letter (Lt)",
	| '\u0061'..'\u007A'	// "Modifier letter (Lm)", "Other letter (Lo)", or "Letter number (Nl)".
	| '\u00AA'
	| '\u00B5'
	| '\u00BA'
	| '\u00C0'..'\u00D6'
	| '\u00D8'..'\u00F6'
	| '\u00F8'..'\u021F'
	| '\u0222'..'\u0233'
	| '\u0250'..'\u02AD'
	| '\u02B0'..'\u02B8'
	| '\u02BB'..'\u02C1'
	| '\u02D0'..'\u02D1'
	| '\u02E0'..'\u02E4'
	| '\u02EE'
	| '\u037A'
	| '\u0386'
	| '\u0388'..'\u038A'
	| '\u038C'
	| '\u038E'..'\u03A1'
	| '\u03A3'..'\u03CE'
	| '\u03D0'..'\u03D7'
	| '\u03DA'..'\u03F3'
	| '\u0400'..'\u0481'
	| '\u048C'..'\u04C4'
	| '\u04C7'..'\u04C8'
	| '\u04CB'..'\u04CC'
	| '\u04D0'..'\u04F5'
	| '\u04F8'..'\u04F9'
	| '\u0531'..'\u0556'
	| '\u0559'
	| '\u0561'..'\u0587'
	| '\u05D0'..'\u05EA'
	| '\u05F0'..'\u05F2'
	| '\u0621'..'\u063A'
	| '\u0640'..'\u064A'
	| '\u0671'..'\u06D3'
	| '\u06D5'
	| '\u06E5'..'\u06E6'
	| '\u06FA'..'\u06FC'
	| '\u0710'
	| '\u0712'..'\u072C'
	| '\u0780'..'\u07A5'
	| '\u0905'..'\u0939'
	| '\u093D'
	| '\u0950'
	| '\u0958'..'\u0961'
	| '\u0985'..'\u098C'
	| '\u098F'..'\u0990'
	| '\u0993'..'\u09A8'
	| '\u09AA'..'\u09B0'
	| '\u09B2'
	| '\u09B6'..'\u09B9'
	| '\u09DC'..'\u09DD'
	| '\u09DF'..'\u09E1'
	| '\u09F0'..'\u09F1'
	| '\u0A05'..'\u0A0A'
	| '\u0A0F'..'\u0A10'
	| '\u0A13'..'\u0A28'
	| '\u0A2A'..'\u0A30'
	| '\u0A32'..'\u0A33'
	| '\u0A35'..'\u0A36'
	| '\u0A38'..'\u0A39'
	| '\u0A59'..'\u0A5C'
	| '\u0A5E'
	| '\u0A72'..'\u0A74'
	| '\u0A85'..'\u0A8B'
	| '\u0A8D'
	| '\u0A8F'..'\u0A91'
	| '\u0A93'..'\u0AA8'
	| '\u0AAA'..'\u0AB0'
	| '\u0AB2'..'\u0AB3'
	| '\u0AB5'..'\u0AB9'
	| '\u0ABD'
	| '\u0AD0'
	| '\u0AE0'
	| '\u0B05'..'\u0B0C'
	| '\u0B0F'..'\u0B10'
	| '\u0B13'..'\u0B28'
	| '\u0B2A'..'\u0B30'
	| '\u0B32'..'\u0B33'
	| '\u0B36'..'\u0B39'
	| '\u0B3D'
	| '\u0B5C'..'\u0B5D'
	| '\u0B5F'..'\u0B61'
	| '\u0B85'..'\u0B8A'
	| '\u0B8E'..'\u0B90'
	| '\u0B92'..'\u0B95'
	| '\u0B99'..'\u0B9A'
	| '\u0B9C'
	| '\u0B9E'..'\u0B9F'
	| '\u0BA3'..'\u0BA4'
	| '\u0BA8'..'\u0BAA'
	| '\u0BAE'..'\u0BB5'
	| '\u0BB7'..'\u0BB9'
	| '\u0C05'..'\u0C0C'
	| '\u0C0E'..'\u0C10'
	| '\u0C12'..'\u0C28'
	| '\u0C2A'..'\u0C33'
	| '\u0C35'..'\u0C39'
	| '\u0C60'..'\u0C61'
	| '\u0C85'..'\u0C8C'
	| '\u0C8E'..'\u0C90'
	| '\u0C92'..'\u0CA8'
	| '\u0CAA'..'\u0CB3'
	| '\u0CB5'..'\u0CB9'
	| '\u0CDE'
	| '\u0CE0'..'\u0CE1'
	| '\u0D05'..'\u0D0C'
	| '\u0D0E'..'\u0D10'
	| '\u0D12'..'\u0D28'
	| '\u0D2A'..'\u0D39'
	| '\u0D60'..'\u0D61'
	| '\u0D85'..'\u0D96'
	| '\u0D9A'..'\u0DB1'
	| '\u0DB3'..'\u0DBB'
	| '\u0DBD'
	| '\u0DC0'..'\u0DC6'
	| '\u0E01'..'\u0E30'
	| '\u0E32'..'\u0E33'
	| '\u0E40'..'\u0E46'
	| '\u0E81'..'\u0E82'
	| '\u0E84'
	| '\u0E87'..'\u0E88'
	| '\u0E8A'
	| '\u0E8D'
	| '\u0E94'..'\u0E97'
	| '\u0E99'..'\u0E9F'
	| '\u0EA1'..'\u0EA3'
	| '\u0EA5'
	| '\u0EA7'
	| '\u0EAA'..'\u0EAB'
	| '\u0EAD'..'\u0EB0'
	| '\u0EB2'..'\u0EB3'
	| '\u0EBD'..'\u0EC4'
	| '\u0EC6'
	| '\u0EDC'..'\u0EDD'
	| '\u0F00'
	| '\u0F40'..'\u0F6A'
	| '\u0F88'..'\u0F8B'
	| '\u1000'..'\u1021'
	| '\u1023'..'\u1027'
	| '\u1029'..'\u102A'
	| '\u1050'..'\u1055'
	| '\u10A0'..'\u10C5'
	| '\u10D0'..'\u10F6'
	| '\u1100'..'\u1159'
	| '\u115F'..'\u11A2'
	| '\u11A8'..'\u11F9'
	| '\u1200'..'\u1206'
	| '\u1208'..'\u1246'
	| '\u1248'
	| '\u124A'..'\u124D'
	| '\u1250'..'\u1256'
	| '\u1258'
	| '\u125A'..'\u125D'
	| '\u1260'..'\u1286'
	| '\u1288'
	| '\u128A'..'\u128D'
	| '\u1290'..'\u12AE'
	| '\u12B0'
	| '\u12B2'..'\u12B5'
	| '\u12B8'..'\u12BE'
	| '\u12C0'
	| '\u12C2'..'\u12C5'
	| '\u12C8'..'\u12CE'
	| '\u12D0'..'\u12D6'
	| '\u12D8'..'\u12EE'
	| '\u12F0'..'\u130E'
	| '\u1310'
	| '\u1312'..'\u1315'
	| '\u1318'..'\u131E'
	| '\u1320'..'\u1346'
	| '\u1348'..'\u135A'
	| '\u13A0'..'\u13B0'
	| '\u13B1'..'\u13F4'
	| '\u1401'..'\u1676'
	| '\u1681'..'\u169A'
	| '\u16A0'..'\u16EA'
	| '\u1780'..'\u17B3'
	| '\u1820'..'\u1877'
	| '\u1880'..'\u18A8'
	| '\u1E00'..'\u1E9B'
	| '\u1EA0'..'\u1EE0'
	| '\u1EE1'..'\u1EF9'
	| '\u1F00'..'\u1F15'
	| '\u1F18'..'\u1F1D'
	| '\u1F20'..'\u1F39'
	| '\u1F3A'..'\u1F45'
	| '\u1F48'..'\u1F4D'
	| '\u1F50'..'\u1F57'
	| '\u1F59'
	| '\u1F5B'
	| '\u1F5D'
	| '\u1F5F'..'\u1F7D'
	| '\u1F80'..'\u1FB4'
	| '\u1FB6'..'\u1FBC'
	| '\u1FBE'
	| '\u1FC2'..'\u1FC4'
	| '\u1FC6'..'\u1FCC'
	| '\u1FD0'..'\u1FD3'
	| '\u1FD6'..'\u1FDB'
	| '\u1FE0'..'\u1FEC'
	| '\u1FF2'..'\u1FF4'
	| '\u1FF6'..'\u1FFC'
	| '\u207F'
	| '\u2102'
	| '\u2107'
	| '\u210A'..'\u2113'
	| '\u2115'
	| '\u2119'..'\u211D'
	| '\u2124'
	| '\u2126'
	| '\u2128'
	| '\u212A'..'\u212D'
	| '\u212F'..'\u2131'
	| '\u2133'..'\u2139'
	| '\u2160'..'\u2183'
	| '\u3005'..'\u3007'
	| '\u3021'..'\u3029'
	| '\u3031'..'\u3035'
	| '\u3038'..'\u303A'
	| '\u3041'..'\u3094'
	| '\u309D'..'\u309E'
	| '\u30A1'..'\u30FA'
	| '\u30FC'..'\u30FE'
	| '\u3105'..'\u312C'
	| '\u3131'..'\u318E'
	| '\u31A0'..'\u31B7'
	| '\u3400'
	| '\u4DB5'
	| '\u4E00'
	| '\u9FA5'
	| '\uA000'..'\uA48C'
	| '\uAC00'
	| '\uD7A3'
	| '\uF900'..'\uFA2D'
	| '\uFB00'..'\uFB06'
	| '\uFB13'..'\uFB17'
	| '\uFB1D'
	| '\uFB1F'..'\uFB28'
	| '\uFB2A'..'\uFB36'
	| '\uFB38'..'\uFB3C'
	| '\uFB3E'
	| '\uFB40'..'\uFB41'
	| '\uFB43'..'\uFB44'
	| '\uFB46'..'\uFBB1'
	| '\uFBD3'..'\uFD3D'
	| '\uFD50'..'\uFD8F'
	| '\uFD92'..'\uFDC7'
	| '\uFDF0'..'\uFDFB'
	| '\uFE70'..'\uFE72'
	| '\uFE74'
	| '\uFE76'..'\uFEFC'
	| '\uFF21'..'\uFF3A'
	| '\uFF41'..'\uFF5A'
	| '\uFF66'..'\uFFBE'
	| '\uFFC2'..'\uFFC7'
	| '\uFFCA'..'\uFFCF'
	| '\uFFD2'..'\uFFD7'
	| '\uFFDA'..'\uFFDC'
	;
	
fragment UnicodeCombiningMark	// Any character in the Unicode categories "Non-spacing mark (Mn)"
	: '\u0300'..'\u034E'	// or "Combining spacing mark (Mc)".
	| '\u0360'..'\u0362'
	| '\u0483'..'\u0486'
	| '\u0591'..'\u05A1'
	| '\u05A3'..'\u05B9'
	| '\u05BB'..'\u05BD'
	| '\u05BF' 
	| '\u05C1'..'\u05C2'
	| '\u05C4'
	| '\u064B'..'\u0655'
	| '\u0670'
	| '\u06D6'..'\u06DC'
	| '\u06DF'..'\u06E4'
	| '\u06E7'..'\u06E8'
	| '\u06EA'..'\u06ED'
	| '\u0711'
	| '\u0730'..'\u074A'
	| '\u07A6'..'\u07B0'
	| '\u0901'..'\u0903'
	| '\u093C'
	| '\u093E'..'\u094D'
	| '\u0951'..'\u0954'
	| '\u0962'..'\u0963'
	| '\u0981'..'\u0983'
	| '\u09BC'..'\u09C4'
	| '\u09C7'..'\u09C8'
	| '\u09CB'..'\u09CD'
	| '\u09D7'
	| '\u09E2'..'\u09E3'
	| '\u0A02'
	| '\u0A3C'
	| '\u0A3E'..'\u0A42'
	| '\u0A47'..'\u0A48'
	| '\u0A4B'..'\u0A4D'
	| '\u0A70'..'\u0A71'
	| '\u0A81'..'\u0A83'
	| '\u0ABC'
	| '\u0ABE'..'\u0AC5'
	| '\u0AC7'..'\u0AC9'
	| '\u0ACB'..'\u0ACD'
	| '\u0B01'..'\u0B03'
	| '\u0B3C'
	| '\u0B3E'..'\u0B43'
	| '\u0B47'..'\u0B48'
	| '\u0B4B'..'\u0B4D'
	| '\u0B56'..'\u0B57'
	| '\u0B82'..'\u0B83'
	| '\u0BBE'..'\u0BC2'
	| '\u0BC6'..'\u0BC8'
	| '\u0BCA'..'\u0BCD'
	| '\u0BD7'
	| '\u0C01'..'\u0C03'
	| '\u0C3E'..'\u0C44'
	| '\u0C46'..'\u0C48'
	| '\u0C4A'..'\u0C4D'
	| '\u0C55'..'\u0C56'
	| '\u0C82'..'\u0C83'
	| '\u0CBE'..'\u0CC4'
	| '\u0CC6'..'\u0CC8'
	| '\u0CCA'..'\u0CCD'
	| '\u0CD5'..'\u0CD6'
	| '\u0D02'..'\u0D03'
	| '\u0D3E'..'\u0D43'
	| '\u0D46'..'\u0D48'
	| '\u0D4A'..'\u0D4D'
	| '\u0D57'
	| '\u0D82'..'\u0D83'
	| '\u0DCA'
	| '\u0DCF'..'\u0DD4'
	| '\u0DD6'
	| '\u0DD8'..'\u0DDF'
	| '\u0DF2'..'\u0DF3'
	| '\u0E31'
	| '\u0E34'..'\u0E3A'
	| '\u0E47'..'\u0E4E'
	| '\u0EB1'
	| '\u0EB4'..'\u0EB9'
	| '\u0EBB'..'\u0EBC'
	| '\u0EC8'..'\u0ECD'
	| '\u0F18'..'\u0F19'
	| '\u0F35'
	| '\u0F37'
	| '\u0F39'
	| '\u0F3E'..'\u0F3F'
	| '\u0F71'..'\u0F84'
	| '\u0F86'..'\u0F87'
	| '\u0F90'..'\u0F97'
	| '\u0F99'..'\u0FBC'
	| '\u0FC6'
	| '\u102C'..'\u1032'
	| '\u1036'..'\u1039'
	| '\u1056'..'\u1059'
	| '\u17B4'..'\u17D3'
	| '\u18A9'
	| '\u20D0'..'\u20DC'
	| '\u20E1'
	| '\u302A'..'\u302F'
	| '\u3099'..'\u309A'
	| '\uFB1E'
	| '\uFE20'..'\uFE23'
	;

fragment UnicodeDigit		// Any character in the Unicode category "Decimal number (Nd)".
	: '\u0030'..'\u0039'
	| '\u0660'..'\u0669'
	| '\u06F0'..'\u06F9'
	| '\u0966'..'\u096F'
	| '\u09E6'..'\u09EF'
	| '\u0A66'..'\u0A6F'
	| '\u0AE6'..'\u0AEF'
	| '\u0B66'..'\u0B6F'
	| '\u0BE7'..'\u0BEF'
	| '\u0C66'..'\u0C6F'
	| '\u0CE6'..'\u0CEF'
	| '\u0D66'..'\u0D6F'
	| '\u0E50'..'\u0E59'
	| '\u0ED0'..'\u0ED9'
	| '\u0F20'..'\u0F29'
	| '\u1040'..'\u1049'
	| '\u1369'..'\u1371'
	| '\u17E0'..'\u17E9'
	| '\u1810'..'\u1819'
	| '\uFF10'..'\uFF19'
	;
	
fragment UnicodeConnectorPunctuation	// Any character in the Unicode category "Connector punctuation (Pc)".
	: '\u005F'
	| '\u203F'..'\u2040'
	| '\u30FB'
	| '\uFE33'..'\uFE34'
	| '\uFE4D'..'\uFE4F'
	| '\uFF3F'
	| '\uFF65'
	;
	
Comment
	: '/*' (options {greedy=false;} : .)* '*/' {$channel=HIDDEN;}
	;

LineComment
	: '//' ~(LineSeparator)* {$channel=HIDDEN;}
	;

LineSeparator
	: '\n'		// Line feed.
	| '\r'		// Carriage return.
	| '\u2028'	// Line separator.
	| '\u2029'	// Paragraph separator.
	;

WhiteSpace // Tab, vertical tab, form feed, space, non-breaking space and any other unicode "space separator".
	: ('\t' | '\v' | '\f' | ' ' | '\u00A0')	{$channel=HIDDEN;}
	;
