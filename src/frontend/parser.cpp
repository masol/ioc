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
#include "frontend/parser.h"
#include "frontend/antlrhelper.h"
#include "parserhelper.h"


namespace ioc{
namespace frontend{

static	ioc::AstNode* ParserStream(pANTLR3_INPUT_STREAM i_input)
{
#define SAFEFREEPTR(ptr)	\
	if(ptr){		\
		ptr->free(ptr);	\
		ptr = NULL;	\
	}

    pjavascriptLexer			    	i_lxr;
    pANTLR3_COMMON_TOKEN_STREAM	    	i_tstream;
    pjavascriptParser			    	i_psr;
	
	ioc::AstNode *pRet = NULL;
	do{
		i_lxr = javascriptLexerNew(i_input);	    // CLexerNew is generated by ANTLR
		if ( i_lxr == NULL )
		{
			//fprintf(stderr, "Unable to create the lexer due to malloc() failure1\n");
			break;
		}
		i_tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(i_lxr));
		if (i_tstream == NULL)
		{
			//fprintf(stderr, "Out of memory trying to allocate token stream\n");
			break;
		}

		i_psr = javascriptParserNew(i_tstream);  // CParserNew is generated by ANTLR3
		if (i_psr == NULL)
		{
			//fprintf(stderr, "Out of memory trying to allocate parser\n");
			break;
		}
		
		javascriptParser_program_return	    i_langAST;

		i_langAST = i_psr->program(i_psr);

		if (i_psr->pParser->rec->state->errorCount > 0)
		{
				fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n",i_psr->pParser->rec->state->errorCount);
				break;
		}
		
		pRet = (AstNode*)i_langAST.ret;
	}while(false);
		
	SAFEFREEPTR(i_psr);
	SAFEFREEPTR(i_tstream);
	SAFEFREEPTR(i_lxr);
#undef SAFEFREEPTR
	return pRet;
}

static	inline	pANTLR3_UINT8	getANTLR3_BUFFERNAME(const char* data)
{
	if(data)
		return (pANTLR3_UINT8)data;
	return (pANTLR3_UINT8)"memory";
}

static	ioc::AstNode* ParserContent(const char* buf,ANTLR3_UINT32 bufsize,const char* buffername)
{
	pANTLR3_INPUT_STREAM	    		i_input;

	i_input = antlr3StringStreamNew((pANTLR3_UINT8)buf,ANTLR3_ENC_8BIT,bufsize,getANTLR3_BUFFERNAME(buffername));
	if(i_input == NULL)
	{
		return NULL;
	}

	ioc::AstNode* ret = ParserStream(i_input);

	i_input->close (i_input);
	
	return ret;
}

static	ioc::AstNode* ParserFile(const std::string &fName)
{

	pANTLR3_INPUT_STREAM	    		i_input;
	
	i_input	= antlr3FileStreamNew((pANTLR3_UINT8)fName.c_str(),ANTLR3_ENC_UTF8);
	if(i_input == NULL)
	{
		//fprintf(stderr, "Unable to open file %s\n", (char *)fName);
		return NULL;
	}
	
	ioc::AstNode* ret = ParserStream(i_input);
	
	i_input->close (i_input);
	
	return ret;
}


bool	Parser::parser(const std::string &fname)
{
	m_root = ParserFile(fname);
	return (m_root!= NULL);
}

bool	Parser::getErrorMessage(std::string& message)const
{
	return false;
}

}//namespace frontend
}//namespace ioc
 
