/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _IQ_H
#define _IQ_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include "prog_def.h"
class Program_data;


/******************************************************************************/
//                            CLASS DEFINITION
/******************************************************************************/


class Instruction_Queue{
	// DATA
private:
	DS q;
	Program_data * pd;
	
	
	// FUNCTIONS
private:
	inline iop_pt access_first(void) const { return (iop_pt) DS_first(q); }
	inline iop_pt access_next (void) const { return (iop_pt) DS_next (q); }
	
public:
	Instruction_Queue(Program_data * prog_data) {
		q = DS_new_list(sizeof(struct iop));
		pd = prog_data;
	}
	~Instruction_Queue(void)  { DS_delete(q); }
	
	
	void Enqueue(
		str_dx      label,
		op_code    op,
		str_dx      target,
		const sym_pt out,
		const sym_pt left,
		const sym_pt right
	);
	
	void Print_iop(FILE * fd, iop_pt iop) const;
	void Dump     (FILE * fd            ) const;
};


#endif // _IQ_H


