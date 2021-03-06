/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "prog_data.h"
#include "proto.h"
#include "errors.h"
#include <string.h>

/**	@file opt.c
 *	Optomize the intermediate representation.
 *	This function converts the two intermediate instruction queues into a single,
 *	optomized, basic-block queue.
 */


/******************************************************************************/
//                               GLOBALS
/******************************************************************************/



/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


static Instruction_Queue * Mk_blk(Instruction_Queue * q){
	Instruction_Queue * blk = NULL;
	iop_pt iop;
	
	msg_print(NULL, V_DEBUG, "\tMk_blk(): start");
	sprintf(err_array, "\tMk_blk(): queue has %u", q->count());
	msg_print(NULL, V_DEBUG, "%s", err_array);
	
	if(!q->isempty()){
		
		msg_print(NULL, V_DEBUG, "\tMk_blk(): the queue is not empty");
		
		// Each block must contain at least one instruction
		iop = q->dq();
		if(!iop)
			crit_error("\tInternal: Mk_blk(): counld not dq from non empty q");
		
		msg_print(NULL, V_INFO, "\tMk_blk(): Making Block");
		
		blk = new Instruction_Queue();
		
		blk->nq(iop);
		
		if(
			iop->op == I_JMP  ||
			iop->op == I_JZ   ||
			iop->op == I_RTRN ||
			iop->op == I_CALL
		);
		else while(( iop = q->first() )){
			if(iop->label != NO_NAME) break; // entry points are leaders
			blk->nq(q->dq());
			if(
				iop->op == I_JMP  ||
				iop->op == I_JZ   ||
				iop->op == I_RTRN ||
				iop->op == I_CALL
			) break;
			// statements after exits are leaders
		}
	}
	else msg_print(NULL, V_INFO, "\tMk_blk(): The queue is empty, no block made");
	
	// recover memory
	q->flush();
	
	msg_print(NULL, V_DEBUG, "\tMk_blk(): stop");
	
	return blk;
}


// Colapse labels

static void Dead_blks(DS block_q){
	/* If a block ends with an unconditional jump, and the next block has no label then the second block must be dead
	*/
	
	/* If a block ends with a jump and the next block starts with its target, then the two lines can be removed
	*/
	
	/* After merging two blocks Liveness() should be run again
	*/
}


// optomize inner loops
static void Inner_loop(Instruction_Queue * blk){
	const char * first_lbl = Program_data::get_string(blk->first()->label);
	const char * last_targ = Program_data::get_string(blk->last()->target);
	
	/*
	If the end of a basic block is a jmp to its head then surely it is an inner loop
	*/
	if( first_lbl && last_targ && !strcmp(first_lbl, last_targ) ){
		msg_print(NULL, V_INFO, "\tInner_loop(): start");
		// aggressively optomize the loop
		msg_print(NULL, V_INFO, "\tInner_loop(): stop");
	}
}


// Determine whether each symbol is live in each instruction
static void Liveness(Instruction_Queue * blk){
	iop_pt iop;
	
	msg_print(NULL, V_DEBUG, "\tLiveness(): start");
	
	iop = blk->last();
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
				Program_data::remove_sym(iop->result->name);
				
				// and the iop
				blk->remove();
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
				Program_data::remove_sym(iop->result->name);
				
				// and the iop
				blk->remove();
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
	} while(( iop = blk->previous() ));
	
	msg_print(NULL, V_DEBUG, "\tLiveness(): stop");
}


/******************************************************************************/
//                             PUBLIC FUNCTION
/******************************************************************************/


void Optomize(Instruction_Queue * inst_q){
	Instruction_Queue * blk_pt;
	
	msg_print(NULL, V_INFO, "Optomize(): start");
	
	if(! inst_q->isempty() ){
		
		// collapse labels
		
		if(make_debug) inst_q->Dump(debug_fd);
		
		while (( blk_pt = Mk_blk(inst_q) )){
		
			#ifdef BLK_ADDR
			sprintf(
				err_array,
				"Optomize(): Mk_blk() returned address: %p",
				(void*) blk_pt
			);
			msg_print(NULL, V_DEBUG, err_array);
			#endif
			
/*			sprintf(*/
/*				err_array,*/
/*				"Optomize(): Printing block of size %u",*/
/*				DS_count(blk)*/
/*			);*/
/*			msg_print(NULL, V_DEBUG, err_array);*/
/*			*/
/*			if(verbosity >= V_DEBUG) Dump_iq(stderr, blk);*/
		
			Liveness(blk_pt);
			Inner_loop(blk_pt);
			
/*			sprintf(*/
/*				err_array,*/
/*				"Block queue has %u, adding one",*/
/*				DS_count(prog->block_q)*/
/*			);*/
/*			msg_print(NULL, V_DEBUG, err_array);*/
			
			Program_data::block_q.nq(blk_pt);
			
/*			sprintf(*/
/*				err_array,*/
/*				"Block queue has %u now",*/
/*				DS_count(prog->block_q)*/
/*			);*/
/*			msg_print(NULL, V_DEBUG, err_array);*/
			
			if( blk_pt != Program_data::block_q.last() )
				msg_print(NULL, V_ERROR,
					"Internal: Queued block does not match first block");
		}
	}
	else msg_print(NULL, V_INFO, "Optomize(): The queue is empty");
	
	delete inst_q;
	
	sprintf(
				err_array,
				"Block queue has %u finally",
				Program_data::block_q.count()
			);
			msg_print(NULL, V_DEBUG, "%s", err_array);
	
	msg_print(NULL, V_INFO, "Optomize(): stop");
}


