/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include <data.h>
#include "scope.h"
#include "proto.h"

typedef struct contxt{
	Instruction_Queue * inst_q;
	sym_pt            scope;
} * contxt_pt;


Scope_Stack::Scope_Stack (Program_data * prog_data){
	stack = DS_new_list(sizeof(struct contxt));
	pd = prog_data;
}

Scope_Stack::~Scope_Stack(void) { DS_delete(stack); }


void Scope_Stack::push(sym_pt sym){
	struct contxt con;
	
	con.inst_q = new Instruction_Queue(pd);
	con.scope  = sym;
	
	DS_push(stack, &con);
}

bool Scope_Stack::pop(void){
	contxt_pt con;
	
	con = (contxt_pt)DS_pop(stack);
	
	if(con){
		if(make_debug) con->inst_q->Dump(debug_fd);
	
		Optomize(pd, con->inst_q);
		if(!con->inst_q->isempty())
			err_msg("Internal: pop_scope(): Optomize() returned non-empty queue");
	
		delete con->inst_q;
		return true;
	}
	else return false;
}

Instruction_Queue * Scope_Stack::instructions(void) const{
	contxt_pt con;
	
	con = (contxt_pt) DS_first(stack);
	if(!con){
		err_msg("Scope_Stack::instructions(): the scope stack is empty");
		return NULL;
	}
	else return con->inst_q;
}

// get the current scope prefix
//inline const char * scope_prefix(void){
//	prg_blk * block_pt;
//	
//	block_pt = (prg_blk*)DS_first(scope_stack);
//	
//	if(block_pt->scope)
//		return dx_to_name(block_pt->scope->name);
//	else return "";
//}



/*
This function returns the correct symbol for the given name in the current scope
*/
//void Scanner::bind(void){
//	static char * buffer;
//	static size_t buf_l;
//	#define BUF_SZ 64
//	
//	char * prefix;
//	size_t name_l;
////	prg_blk * block_pt;
////	
////	// initializate the buffer
////	if(!buffer){
////		buffer = (char*)malloc(BUF_SZ);
////		buf_l = BUF_SZ;
////	}
////	
////	yysymbol = NULL;
////	
////	block_pt = (prg_blk*)DS_first(scope_stack);
////	
////	// resize the buffer if necessary
////	if(block_pt->scope){
////		prefix = dx_to_name(block_pt->scope->name);
////		name_l = strlen(prefix) + strlen(yytext) + 1;
////		if(name_l > buf_l) buffer = realloc(buffer, name_l);
////	}
////	
////	// search the scope stack
////	while ( block_pt->scope ){
////		prefix = dx_to_name(block_pt->scope->name);
////		strncpy(buffer, prefix, strlen(prefix));
////		strncat(buffer, yytext, strlen(yytext));
////		if(( yysymbol = DS_find(symbols, buffer) )) break;
////		block_pt = (prg_blk*)DS_next(scope_stack);
////	}
////	
////	// if we didn't find anything, check the global scope
////	if(!yysymbol) yysymbol = DS_find(symbols, yytext);
//}


