/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"

/**	@file opt.c
 *	Optomize the intermediate representation.
 *	This function converts the two intermediate instruction queues into a single,
 *	optomized, basic-block queue.
 */


/******************************************************************************/
//                               DEFINITIONS
/******************************************************************************/



/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


static DS Mk_blk(DS q){
	DS blk;
	icmd * iop;
	
	// Each block must contain at least one instruction
	iop = (icmd*)DS_dq(q);
	if(!iop) return NULL;
	
	info_msg("Making Block");
	
	blk = (DS) DS_new_list(sizeof(icmd));
	
	DS_nq(blk, iop);
	
	while(( iop = (icmd*)DS_first(q) )){
		if(iop->label != NO_NAME) break; // entry points are leaders
		DS_nq(blk, DS_dq(q));
		if(iop->op == I_JMP || iop->op == I_JZ || iop->op == I_RTRN) break;
		// statements after exits are leaders
	}
	
	// recover memory
	DS_flush(q);
	
	return blk;
}

//static void Dead_code(DS blk){}
//static void Remove_dead_symbols(){}

static void Next_use(DS blk){
	icmd * iop;
	
	iop = (icmd*)DS_first(blk);
	if(!iop) crit_error("Internal: Next_use() received an empty block");
	
	do {
		if(iop->op > I_DEC){ // if a binary op
			if(
				!iop->arg2_lit &&
				iop->arg2.symbol->temp &&
				!iop->arg2.symbol->live
			)
			iop->arg2_live = false;
			else iop->arg2_live = true;
		}
		
		if(iop->op != I_NOP){
			// set liveness in iop
			if(iop->result->temp && !iop->result->live)
				iop->result_live = false;
			else iop->result_live = true;
			
			if(
				!iop->arg1_lit &&
				iop->arg1.symbol->temp &&
				!iop->arg1.symbol->live
			)
				iop->arg1_live = false;
			else iop->arg1_live = true;
			
			
			
			// set liveness in symbols
			if (iop->result->temp) iop->result->live = false;
			else iop->result->live = true;
		
			if(!iop->arg1_lit) iop->arg1.symbol->live = true;
			if(iop->op > I_DEC && !iop->arg2_lit) iop->arg2.symbol->live = true;
		}
	} while(( iop = (icmd*)DS_next(blk) ));
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void Optomize(Program_data prog){
	DS blk;
	
	info_msg("Optomizing...");
	
	while (( blk = Mk_blk(prog.main_q) )){
		Next_use(blk);
		DS_nq(prog.block_q, blk);
	}
	
	while (( blk = Mk_blk(prog.sub_q) )){
		Next_use(blk);
		DS_nq(prog.block_q, blk);
	}
	
	info_msg("Finished Optomizing");
}


