/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "scanner.h" // interface for the flex component
#include "lex.h"

/******************************************************************************/
//                            SCANNER FUNCTIONS
/******************************************************************************/


/*
This function returns the correct symbol for the given name in the current scope
*/
void Scanner::Bind(void){
	static char * buffer;
	static size_t buf_l;
	#define BUF_SZ 64
	
	char * prefix;
	size_t name_l;
//	prg_blk * block_pt;
//	
//	// initializate the buffer
//	if(!buffer){
//		buffer = (char*)malloc(BUF_SZ);
//		buf_l = BUF_SZ;
//	}
//	
//	yysymbol = NULL;
//	
//	block_pt = (prg_blk*)DS_first(scope_stack);
//	
//	// resize the buffer if necessary
//	if(block_pt->scope){
//		prefix = dx_to_name(block_pt->scope->name);
//		name_l = strlen(prefix) + strlen(yytext) + 1;
//		if(name_l > buf_l) buffer = realloc(buffer, name_l);
//	}
//	
//	// search the scope stack
//	while ( block_pt->scope ){
//		prefix = dx_to_name(block_pt->scope->name);
//		strncpy(buffer, prefix, strlen(prefix));
//		strncat(buffer, yytext, strlen(yytext));
//		if(( yysymbol = DS_find(symbols, buffer) )) break;
//		block_pt = (prg_blk*)DS_next(scope_stack);
//	}
//	
//	// if we didn't find anything, check the global scope
//	if(!yysymbol) yysymbol = DS_find(symbols, yytext);
}

void Scanner::next_token(void){
	static unsigned int tabs_last_ln;
	
	if(tabs_last_ln == tabs_this_ln) {
		scan_token = yylex();
		return;
	}
}


