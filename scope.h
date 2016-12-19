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
	
	
	static umax         read_num (const char * str);
	static const char * read_str (const char * str);
	static umax         read_char(const char * str);
	
public:
	 Scope_Stack(void);
	~Scope_Stack(void);
	
	void                push        (sym_pt       sym )      ;
	Instruction_Queue * pop         (void             )      ;
	void                bindop      (void             ) const;
	Instruction_Queue * instructions(void             ) const;
	
	static sym_pt       bind(token_t &token, const char * name);
};


#endif // _SCOPE_H


