/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _BLOCK_Q_H
#define _BLOCK_Q_H


#include "prog_def.h"
#include "errors.h"
#include "iq.h"

class Block_Queue{
private:
	DS bq;
public:
	Block_Queue(void) { bq = (DS) DS_new_list(sizeof(Instruction_Queue)); }
	~Block_Queue(void) { DS_delete(bq); }
	
	inline bool isempty(void) const { return DS_isempty(bq); }
	inline uint count  (void) const { return DS_count  (bq); }
	
	inline Instruction_Queue * first(void) const {
		return (Instruction_Queue*) DS_first(bq);
	}
	inline Instruction_Queue * next(void) const {
		return (Instruction_Queue*) DS_next(bq);
	}
	inline Instruction_Queue * nq(Instruction_Queue * q){
		return (Instruction_Queue*) DS_nq(bq, q);
	}
	inline Instruction_Queue * dq(void){
		return (Instruction_Queue*) DS_dq(bq);
	}
	inline void Dump(FILE * fd) const;
};


#endif // _BLOCK_Q_H


