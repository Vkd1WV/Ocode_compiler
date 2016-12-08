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


void Optomize(Program_data * prog){
	DS blk;
	
	info_msg("Optomize(): start");
	
	if(!DS_isempty(prog->main_q)){
		debug_msg("Optomize(): main_q");
		while (( blk = Mk_blk(prog->main_q) )){
		
			#ifdef BLK_ADDR
			sprintf(
				err_array,
				"Optomize(): Mk_blk() returned address: %p",
				(void*) blk
			);
			debug_msg(err_array);
			#endif
			
			sprintf(
				err_array,
				"Optomize(): Printing block of size %u",
				DS_count(blk)
			);
			debug_msg(err_array);
			
			if(verbosity >= V_DEBUG) Dump_iq(stderr, blk);
		
			//Next_use(blk);
			sprintf(
				err_array,
				"Block queue has %u, adding one",
				DS_count(prog->block_q)
			);
			debug_msg(err_array);
			
			DS_nq(prog->block_q, &blk);
			
			sprintf(
				err_array,
				"Block queue has %u now",
				DS_count(prog->block_q)
			);
			debug_msg(err_array);
			
			if( blk != * (DS_pt) DS_first(prog->block_q) ){
				err_msg("Queued block does not match first block");
			}
			else info_msg("\t\tEURIKA they match");
			
		}
	}
	else info_msg("Optomize(): The main queue is empty");
	
	if(!DS_isempty(prog->sub_q)){
		debug_msg("sub_q");
		while (( blk = Mk_blk(prog->sub_q) )){
		
			if (verbosity >= V_DEBUG){
				fprintf(stderr, "\nPrinting block of size %u.\n", DS_count(blk));
				Dump_iq(stderr, blk);
			}
		
			//Next_use(blk);
			DS_nq(prog->block_q, blk);
		}
	}
	else info_msg("Optomize(): The sub queue is empty");
	
	
	sprintf(
				err_array,
				"Block queue has %u finally",
				DS_count(prog->block_q)
			);
			debug_msg(err_array);
	
	info_msg("Optomize(): stop");
}


