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

#define GLOBAL_SCOPE NULL

class Scope_Stack{
private:
	DS stack;
	Program_data * pd;
	
public:
	 Scope_Stack(Program_data * prog_data);
	~Scope_Stack(void);
	
	void                push        (sym_pt       sym )      ;
	bool                pop         (void             )      ;
	sym_pt              bind        (const char * name) const;
	void                bindop      (void             ) const;
	Instruction_Queue * instructions(void             ) const;
};


#endif // _SCOPE_H


