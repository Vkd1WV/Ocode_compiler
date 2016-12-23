/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include <data.h>
#include "scope.h"
#include "errors.h"
#include "scanner.h"
#include "prog_data.h"
#include <string.h>


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


typedef struct contxt{
	Instruction_Queue * inst_q;
	sym_pt              scope;
} * contxt_pt;


/******************************************************************************/
//                            STATIC MEMBERS
/******************************************************************************/


DS     Scope_Stack::stack;
char * Scope_Stack::buffer;
size_t Scope_Stack::buf_l;


/******************************************************************************/
//                           MEMBER FUNCTIONS
/******************************************************************************/


Scope_Stack::Scope_Stack (void){
	stack = DS_new_list(sizeof(struct contxt));
	buffer = (char*) malloc(50); // anything will work here
	buf_l = 50;
}

Scope_Stack::~Scope_Stack(void){
	contxt_pt pt;
	
	debug_msg("Scope_Stack::~Scope_Stack(): start");
	
	// just make sure the stack is empty
	while(( pt = (contxt_pt)DS_pop(stack) )){
		err_msg("Internal: Scope_Stack::~Scope_Stack(): the stack isn't empty");
		delete pt->inst_q;
	}
	
	DS_delete(stack);
	
	free(buffer);
	
	debug_msg("Scope_Stack::~Scope_Stack(): stop");
}


void Scope_Stack::push(sym_pt sym){
	struct contxt con;
	
	con.inst_q = new Instruction_Queue();
	con.scope  = sym;
	
	DS_push(stack, &con);
}

Instruction_Queue * Scope_Stack::pop(void){
	contxt_pt con;
	
	con = (contxt_pt)DS_pop(stack);
	if(con) return con->inst_q;
	else return NULL;
}

void Scope_Stack::nq_inst(
	str_dx       a,
	op_code      b,
	str_dx       c,
	const sym_pt d,
	const sym_pt e,
	const sym_pt f
){
	contxt_pt con;
	
	con = (contxt_pt) DS_first(stack);
	if(!con)
		crit_error("Scope_Stack::instructions(): the scope stack is empty");
	else con->inst_q->add_inst(a, b, c, d, e, f);
}


// get the current scope prefix
const char * Scope_Stack::prefix(void){
	contxt_pt con;
	
	con = (contxt_pt) DS_first(stack);
	if(!con) crit_error("Scope_Stack::prefix(): no context");
	
	if(con->scope) return Program_data::get_string(con->scope->name);
	else return "";
}


// uses Scanner::text() as the name
	// checks Scanner::token() == T_NAME
	// inserts sym into the symbol table
void Scope_Stack::add_sym(sym_pt &sym){
	size_t length;
	
	length = strlen(prefix()) + Scanner::length() +1;
	
	// resize the buffer
	if(length > buf_l){
		buffer = (char*) realloc(buffer, length);
		buf_l = length;
	}
	
	strcpy(buffer, prefix());
	strcat(buffer, Scanner::text());
	
	sym->name = Program_data::add_string(buffer);
	sym->short_name = sym->name + strlen(prefix());
	
	Program_data::add_symbol(sym);
	
	Scanner::next_token();
}


sym_pt Scope_Stack::bind(token_t &token, const char * name){
	sym_pt sym=NULL;
	
	if (token == T_NAME){
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
	}
	else return NULL;
	
	return sym;
}


void Scope_Stack::emit_cmnt(const char * cmnt){
	nq_inst(NO_NAME, I_NOP, Program_data::add_string(cmnt), NULL, NULL, NULL);
}

void Scope_Stack::emit_lbl (const str_dx lbl, const char * cmnt){
	if(!cmnt) nq_inst(lbl, I_NOP, NO_NAME, NULL, NULL, NULL);
	else
		nq_inst(lbl, I_NOP, Program_data::add_string(cmnt), NULL, NULL, NULL);
}

void Scope_Stack::emit_jump(const str_dx target, const sym_pt condition){
	nq_inst(NO_NAME, I_JMP, target, NULL, condition, NULL);
}

void Scope_Stack::emit_jz  (const str_dx target, const sym_pt condition){
	nq_inst(NO_NAME, I_JZ, target, NULL, condition, NULL);
}

void Scope_Stack::emit_op(
	const op_code op,
	const sym_pt  result,
	const sym_pt  arg1,
	const sym_pt  arg2
){
	nq_inst(NO_NAME, op, NO_NAME, result, arg1, arg2);
}


