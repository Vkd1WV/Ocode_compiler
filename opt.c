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
	DS blk = NULL;
	iop_pt iop;
	
	debug_msg("\tMk_blk(): start");
	sprintf(err_array, "\tMk_blk(): queue has %u", DS_count(q));
	debug_msg(err_array);
	
	if(!DS_isempty(q)){
		
		debug_msg("\tMk_blk(): the queue is not empty");
		
		// Each block must contain at least one instruction
		iop = (iop_pt)DS_dq(q);
		if(!iop) {
			err_msg("\tInternal: Mk_blk(): counld not dq from non empty q");
			return NULL;
		}
	
		info_msg("\tMk_blk(): Making Block");
	
		blk = (DS) DS_new_list(sizeof(icmd));
	
		DS_nq(blk, iop);
	
		while(( iop = (iop_pt)DS_first(q) )){
			if(iop->label != NO_NAME) break; // entry points are leaders
			DS_nq(blk, DS_dq(q));
			if(
				iop->op == I_JMP  ||
				iop->op == I_JZ   ||
				iop->op == I_RTRN ||
				iop->op == I_CALL
			) break;
			// statements after exits are leaders
		}
	}
	else info_msg("\tMk_blk(): The queue is empty, no block made");
	
	// recover memory
	DS_flush(q);
	
	debug_msg("\tMk_blk(): stop");
	
	return blk;
}


// Colapse labels

static void Dead_blks(DS block_q){
	/* If a block ends with an unconditional jump, and the next block has no label then it must be dead
	*/
	
	/* If a block ends with a jump and the next block starts with its target, then the two lines can be removed
	*/
}


// optomize inner loops
static void Inner_loop(DS blk){
	char * first_lbl = dx_to_name(((iop_pt) DS_first(blk))->label );
	char * last_targ = dx_to_name(((iop_pt) DS_last (blk))->target);
	
	/*
	If the end of a basic block is a jmp to its head then surely it is an inner loop
	*/
	if( first_lbl && last_targ && !strcmp(first_lbl, last_targ) ){
		info_msg("\tInner_loop(): start");
		// aggressively optomize the loop
		info_msg("\tInner_loop(): stop");
	}
}


// Determine whether each symbol is live in each instruction
static void Liveness(DS blk, DS symbols){
	iop_pt iop;
	
	debug_msg("\tLiveness(): start");
	
	iop = (iop_pt)DS_last(blk);
	if(!iop) crit_error("Internal: Liveness() received an empty block");
	
	do {
		switch(iop->op){
		case I_NOP :
		case I_CALL:
		case I_PROC: // no arguments
			break;
		
		case I_ASS :
		case I_REF :
		case I_DREF:
		case I_NEG :
		case I_NOT :
		case I_INV :
		case I_INC :
		case I_DEC : // Unary Ops
			
			// if the result is dead remove the op
			if(iop->result->temp && !iop->result->live){
				// remove the temp symbol
				if(DS_find(symbols, dx_to_name(iop->result->name)))
					DS_remove(symbols);
				
				// and the iop
				DS_remove(blk);
				break;
			}
			
			// now we know the result is live
			iop->result_live = true;
			iop->result->live = false;
			
			if(!iop->arg1_lit && iop->arg1.symbol->live) iop->arg1_live = true;
			if(!iop->arg1_lit) iop->arg1.symbol->live = true;
			
			break;
		
		case I_MUL:
		case I_DIV:
		case I_MOD:
		case I_EXP:
		case I_LSH:
		case I_RSH:
		case I_ADD :
		case I_SUB :
		case I_BAND:
		case I_BOR :
		case I_XOR :
		case I_EQ :
		case I_NEQ:
		case I_LT :
		case I_GT :
		case I_LTE:
		case I_GTE:
		case I_AND:
		case I_OR : // Binary Ops
			
			// if the result is dead remove the op
			if(iop->result->temp && !iop->result->live){
				// remove the temp symbol
				if(DS_find(symbols, dx_to_name(iop->result->name)))
					DS_remove(symbols);
				
				// and the iop
				DS_remove(blk);
				break;
			}
			
			// now we know the result is live
			iop->result_live = true;
			iop->result->live = false;
			
			if(!iop->arg1_lit && iop->arg1.symbol->live) iop->arg1_live = true;
			if(!iop->arg2_lit && iop->arg2.symbol->live) iop->arg2_live = true;
			
			if(!iop->arg1_lit) iop->arg1.symbol->live = true;
			if(!iop->arg2_lit) iop->arg2.symbol->live = true;
			
			break;
		
		case I_JMP :
		case I_JZ  :
		case I_RTRN: // treat them like optional unaries
			if(!iop->arg1_lit && iop->arg1.symbol)
				iop->arg1.symbol->live = true;
			break;
		
		case NUM_I_CODES:
		default: crit_error("Liveness(): got a bad op");
		}
	} while(( iop = (icmd*)DS_previous(blk) ));
	
	debug_msg("\tLiveness(): stop");
}


/******************************************************************************/
//                             PUBLIC FUNCTION
/******************************************************************************/


void Optomize(Program_data * prog, DS inst_q){
	DS blk;
	
	info_msg("Optomize(): start");
	
	if(!DS_isempty(inst_q)){
		debug_msg("Optomize(): start");
		while (( blk = Mk_blk(inst_q) )){
		
			#ifdef BLK_ADDR
			sprintf(
				err_array,
				"Optomize(): Mk_blk() returned address: %p",
				(void*) blk
			);
			debug_msg(err_array);
			#endif
			
/*			sprintf(*/
/*				err_array,*/
/*				"Optomize(): Printing block of size %u",*/
/*				DS_count(blk)*/
/*			);*/
/*			debug_msg(err_array);*/
/*			*/
/*			if(verbosity >= V_DEBUG) Dump_iq(stderr, blk);*/
		
			Liveness(blk, prog->symbols);
			Inner_loop(blk);
			
/*			sprintf(*/
/*				err_array,*/
/*				"Block queue has %u, adding one",*/
/*				DS_count(prog->block_q)*/
/*			);*/
/*			debug_msg(err_array);*/
			
			DS_nq(prog->block_q, &blk);
			
/*			sprintf(*/
/*				err_array,*/
/*				"Block queue has %u now",*/
/*				DS_count(prog->block_q)*/
/*			);*/
/*			debug_msg(err_array);*/
			
			if( blk != * (DS_pt) DS_last(prog->block_q) )
				err_msg("Internal: Queued block does not match first block");
		}
	}
	else info_msg("Optomize(): The queue is empty");
	
	sprintf(
				err_array,
				"Block queue has %u finally",
				DS_count(prog->block_q)
			);
			debug_msg(err_array);
	
	info_msg("Optomize(): stop");
}


