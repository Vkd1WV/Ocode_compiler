/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _SCOPE_H
#define _SCOPE_H


#include "prog_data.h"
#include "token.h"

#define GLOBAL_SCOPE NULL

class Scope_Stack{
private:
	static DS stack;
	
	
	static void nq_inst(
		str_dx       a,
		op_code      b,
		str_dx       c,
		const sym_pt d,
		const sym_pt e,
		const sym_pt f
	);
	
public:
	 Scope_Stack(void);
	~Scope_Stack(void);
	
	static void                push        (sym_pt       sym );
	static Instruction_Queue * pop         (void             );
	
	
	
	static sym_pt bind  (token_t &token, const char * name);
	static void   bindop(void             );
	
	static void emit_cmnt(const char * cmnt);
	static void emit_lbl (const str_dx lbl, const char * cmnt);
	static void emit_jump(const str_dx target, const sym_pt condition);
	static void emit_jz  (const str_dx target, const sym_pt condition);
	static void emit_op  (
		const op_code op,
		const sym_pt  result,
		const sym_pt  arg1,
		const sym_pt  arg2
	);
};


#endif // _SCOPE_H


