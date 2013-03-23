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
#include "frontend/astvisitor.h"
#include "utils/log.h"


namespace ioc{

unsigned AstNode::current_id_ = 0;
const char* PropertyReferenceSuffix::sv_name_name = "name";
const char* ContinueStatement::sv_identifier_name = "label";
const char* BreakStatement::sv_identifier_name = "label";
const char* LabelledStatement::sv_identifier_name = "name";
const char* Assignment::sv_operator_name = "operator";
const char* Call::sv_name_name = "name";
const char* CompareOperation::sv_operater_name = "operator";
const char* BinaryOperation::sv_operater_name = "operator";
const char* VariableProxy::sv_is_declar_name = "declare";
const char* VariableProxy::sv_identify_name = "identify";
const char* VariableProxy::sv_true_value = "true";
const char* VariableProxy::sv_false_value = "false";
const char* StringLiteral::sv_literal_name = "literal";
const char* NumberLiteral::sv_literal_name = "literal";
const char* BooleanLiteral::sv_literal_name = "literal";
const char* BooleanLiteral::sv_true_value = "true";
const char* BooleanLiteral::sv_false_value = "false";
const char* NullLiteral::sv_literal_name = "literal";
const char* FunctionDeclaration::sv_name_name = "name";
const char* FunctionExpression::sv_name_name = "name";
const char* UnaryOperation::sv_operator_name = "operator";
const char* UnaryOperation::sv_isfront_name = "isfront";

void AstNode::Traversal(AstVisitor* v)
{
    v->internalBeginTraversal(this);
    size_t childCount = this->childrenCount();
    for(size_t i = 0; i <childCount; i++) {
        AstNode* pNode = this->getChildren(i);
        if(pNode) {
            pNode->Traversal(v);
        }
    }
    v->internalEndTraversal(this);
}

void Assignment::opName(const std::string &name)
{
  opName_ = name;
  if(name == "=") {
    m_operator = ioc::Assignment::T_EQU;
  }
  else if(name == "*=") {
    m_operator = ioc::Assignment::T_MUL;
  }
  else if(name == "/=") {
    m_operator = ioc::Assignment::T_DIV;
  }
  else if(name == "%=") {
    m_operator = ioc::Assignment::T_MOD;
  }
  else if(name == "+=") {
    m_operator = ioc::Assignment::T_ADD;
  }
  else if(name == "-=") {
    m_operator = ioc::Assignment::T_SUB;
  }
  else if(name == "<<=") {
    m_operator = ioc::Assignment::T_SHL;
  }
  else if(name == ">>=") {
    m_operator = ioc::Assignment::T_SHR;
  }
  else if(name == ">>>=") {
    m_operator = ioc::Assignment::T_SHR_ZEROFILL;
  }
  else if(name == "&=") {
    m_operator = ioc::Assignment::T_AND;
  }
  else if(name == "^=") {
    m_operator = ioc::Assignment::T_XOR;
  }
  else if(name == "|=") {
    m_operator = ioc::Assignment::T_OR;
  }else{
    IOC_ASSERT(false && "can not recognize operator name");
  }
}


void CompareOperation::opName(const std::string &name)
{
  opName_ = name; 
  if(name == "<") {
    m_operator = ioc::CompareOperation::T_LT;
  }
  else if(name == ">") {
    m_operator = ioc::CompareOperation::T_GT;
  }
  else if(name == "==") {
    m_operator = ioc::CompareOperation::T_EQU;
  }
  else if(name == "!=") {
    m_operator = ioc::CompareOperation::T_NOTEQU;
  }
  else if(name == "<=") {
    m_operator = ioc::CompareOperation::T_LE;
  }
  else if(name == ">=") {
    m_operator = ioc::CompareOperation::T_GE;
  }
  else if(name == "===") {
    m_operator = ioc::CompareOperation::T_EXEQU;
  }
  else if(name == "!==") {
    m_operator = ioc::CompareOperation::T_EXNOTEQU;
  }
  else if(name == "instanceof") {
    m_operator = ioc::CompareOperation::T_INSTANCEOF;
  }
  else if(name == "in") {
    m_operator = ioc::CompareOperation::T_IN;
  }else{
    IOC_ASSERT(false && "can not recognize operator name");
  }
}

void
BinaryOperation::opName(const std::string &name)
{
  opName_ = name;
  if(name == "+") {
    m_operator = ioc::BinaryOperation::T_ADD;
  }
  else if(name == "-") {
    m_operator = ioc::BinaryOperation::T_SUB;
  }
  else if(name == "*") {
    m_operator = ioc::BinaryOperation::T_MUL;
  }
  else if(name == "/") {
    m_operator = ioc::BinaryOperation::T_DIV;
  }
  else if(name == "%") {
    m_operator = ioc::BinaryOperation::T_MOD;
  }
  else if(name == "|") {
    m_operator = ioc::BinaryOperation::T_BIT_OR;
  }
  else if(name == "^") {
    m_operator = ioc::BinaryOperation::T_BIT_XOR;
  }
  else if(name == "&") {
    m_operator = ioc::BinaryOperation::T_BIT_AND;
  }
  else if(name == "||") {
    m_operator = ioc::BinaryOperation::T_OR;
  }
  else if(name == "&&") {
    m_operator = ioc::BinaryOperation::T_AND;
  }
  else if(name == "<<") {
    m_operator = ioc::BinaryOperation::T_SHL;
  }
  else if(name == ">>") {
    m_operator = ioc::BinaryOperation::T_SHR;
  }
  else if(name == ">>>") {
    m_operator = ioc::BinaryOperation::T_SAR;
  }
  // Special operator.
  else if(name == ",") {
    m_operator = ioc::BinaryOperation::T_COMMA;
  }else{
    IOC_ASSERT(false && "can not recognize operator name");
  }
}


void UnaryOperation::opName(const std::string &name)
{
  opName_ = name;
  // Single char op.
  if(name.length() == 1)
  {
    switch(name[0])
    {
    // With/without `+`, they are the same. So do nothing
    case '+':
      {
	m_operator = ioc::UnaryOperation::T_PLUS;
      }
      break;
    case '-':
      {
	/**
	* "-a" => "0-a"
	* "-(a+b)" => "0-(a+b)"
	* -1和-1.1直接修改节点。
	**/
	// -1
	m_operator = ioc::UnaryOperation::T_MINUS;
      }
      break;
    case '~':
      {
	m_operator = ioc::UnaryOperation::T_TILDE;
      }
      break;
    case '!':
      {
	m_operator = ioc::UnaryOperation::T_NOT;
      }
      break;
    }

  // Double/Multi chars op.
  }else{
    if(name == "++")
    {
      m_operator = ioc::UnaryOperation::T_ADDADD;
    }
    else if(name == "--")
    {
      m_operator = ioc::UnaryOperation::T_SUBSUB;
    }
    else if(name == "delete")
    {
      m_operator = ioc::UnaryOperation::K_DELETE;
    }
    else if(name == "void")
    {
      m_operator = ioc::UnaryOperation::K_VOID;
    }
    else if(name == "typeof")
    {
      m_operator = ioc::UnaryOperation::K_TYPEOF;
    }
    else
    {
      IOC_LOG_SEV(DEBUG) << "Operator [" << name << "] is unknown!!";
    }
  }  
}


} //end namespace ioc
