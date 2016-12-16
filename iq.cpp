/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "iq.h"
#include "prog_data.h"


void Instruction_Queue::Enqueue(
	str_dx      label,
	op_code    op,
	str_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
){
	struct iop iop[1];
	#ifdef IOP_ADDR
		void * temp;
	#endif
	
	iop->label  = label;
	iop->op     = op;
	iop->target = target;
	iop->result = out;
	
	// just to be safe initializations
	iop->result_live = false;
	iop->arg1_live   = false;
	iop->arg2_live   = false;
	iop->arg1.symbol = NULL;
	iop->arg2.symbol = NULL;
	iop->arg1_lit    = false;
	iop->arg2_lit    = false;
	
	#ifdef DBG_EMIT_IOP
	debug_sym("emit_iop()        : out         ", out);
	debug_sym("emit_iop()        : left        ", left);
	debug_sym("emit_iop()        : right       ", right);
	#endif
	
	switch (op){
	case I_NOP :
	case I_RTRN:
		break;
	
	// Binaries
	case I_MUL :
	case I_DIV :
	case I_MOD :
	case I_EXP :
	case I_LSH :
	case I_RSH :
	case I_ADD :
	case I_SUB :
	case I_BAND:
	case I_BOR :
	case I_XOR :
	case I_EQ  :
	case I_NEQ :
	case I_LT  :
	case I_GT  :
	case I_LTE :
	case I_GTE :
	case I_AND :
	case I_OR  : // stuff for binaries only
		if (right->type == st_lit_int){
			iop->arg2_lit   = true;
			iop->arg2.value = right->value;
			
			// remove the lit symbol
			pd->remove_sym(right->name);
		}
		else iop->arg2.symbol = right;
		
	// Unaries
	case I_ASS :
	case I_REF :
	case I_DREF:
	case I_NEG :
	case I_NOT :
	case I_INV : // stuff for unaries and binaries
		if (left->type == st_lit_int){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			
			// remove the lit symbol
			pd->remove_sym(left->name);
		}
		else iop->arg1.symbol = left;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left && left->type == st_lit_int){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			
			// remove the lit symbol
			pd->remove_sym(left->name);
		}
		else iop->arg1.symbol = left;
		break;
		
	case I_INC :
	case I_DEC :
	case I_CALL:
	
	case NUM_I_CODES:
	default:
		sprintf(err_array,
			"Internal: emit_iop() called with cmd = %s",
			op_code_dex[op]
		);
		crit_error(err_array);
	}
	
	#ifdef DBG_EMIT_IOP
	debug_iop("emit_iop()        :Adding iop   ", iop);
	#endif
	
	
/*	if(scope_pos){ // we are inside a sub structure of some kind*/
/*		#ifdef IOP_ADDR*/
/*			temp=*/
/*		#endif*/
/*		DS_nq(sub_inst_q, iop);*/
/*	}*/
/*	else {*/
	
	// queue up this operation
	#ifdef IOP_ADDR
		temp=
	#endif
	DS_nq(q, iop);
	
	
	#ifdef IOP_ADDR
	sprintf(err_array, "iop saved at: %p", temp);
	debug_msg(err_array);
	#endif
	
}

void Instruction_Queue::Print_iop(FILE * fd, iop_pt iop) const {
	const char *result, *arg1, *arg2;
	const char *none = "NONE", *lit = "lit";
	
	if(!iop) fputs("Print_icmd(): NULL\n", fd);
	else{
		if(iop->target != NO_NAME) result = pd->get_string(iop->target);
		else if(iop->result) result = pd->get_string(iop->result->name);
		else result = none;
		
		if(iop->arg1_lit) arg1 = lit;
		else if (iop->arg1.symbol) arg1 = pd->get_string(iop->arg1.symbol->name);
		else arg1 = none;
		
		if(iop->arg2_lit) arg2 = lit;
		else if (iop->arg2.symbol) arg2 = pd->get_string(iop->arg2.symbol->name);
		else arg2 = none;
		
		fprintf(fd, "%6s:\t%s\t%c %6s\t%c %6s\t%c %6s\n",
			pd->get_string(iop->label),
			op_code_dex[iop->op],
			iop->result_live? 'l' : 'd',
			result,
			iop->arg1_live? 'l' : 'd',
			arg1,
			iop->arg2_live? 'l' : 'd',
			arg2
		);
		
	}
}

void Instruction_Queue::Dump(FILE * fd) const {
	iop_pt iop;
	
	//debug_msg("\tDump_iq(): start");
	if (!fd) err_msg("Internal: Dump_iq(): received NULL file descriptor");
	
	#ifdef FLUSH_FILES
		fflush(fd);
	#endif
	
	if( !DS_isempty(q) ){
		iop = this->access_first();
		do {
			#ifdef IOP_ADDR
			sprintf(err_array, "Printing iop at: %p", (void*)iop);
			debug_msg(err_array);
			#endif
		
			Print_iop(fd, iop);
			
			#ifdef FLUSH_FILES
				fflush(fd);
			#endif
		} while (( iop = this->access_next() ));
	}
	else {
		info_msg("\tDump_iq(): The queue is empty");
		fputs("Empty\n", fd);
	}
	
	//debug_msg("\tDump_iq(): stop");
}


