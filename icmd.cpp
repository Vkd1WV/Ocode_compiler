
#include "compiler.h"
#include "icmd.h"
#include "icmd.hpp"

const char * byte_code_dex[NUM_I_CODES] = {
	"I_NOP", "I_ASS", "I_REF", "I_DREF", "I_NEG", "I_NOT" , "I_INV" , "I_INC",
	"I_DEC",
	"I_MUL",
	"I_DIV", "I_MOD", "I_EXP", "I_LSH" , "I_RSH", "I_ADD" , "I_SUB" , "I_BAND",
	"I_BOR", "I_XOR", "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  , "I_LTE" , "I_GTE" ,
	"I_AND", "I_OR" , "I_JMP", "I_JZ"  , "I_BLK", "I_EBLK", "I_CALL", "I_RTRN"
};

inline void icmd::Print(void){
	char first[16], second[16];
	
//	if (this->result) result = dx_to_name(this->result->name);
//	else result = dx_to_name(this->target);
	
	if (this->arg1) strncpy(arg1, dx_to_name(this->arg1.s->name), 16);
	else sprintf(arg1, "0x%llx", this->arg1_lit);

	else arg1[0] = '\0';
	
	if (this->arg2){
		if(this->arg2_lit) sprintf(arg1, "0x%llx", this->arg2.l);
		else strncpy(arg2, dx_to_name(this->arg2.s->name), 16);
	}
	else arg1[0] = '\0';
	
	printf("%4s:\t%s\t%4s\t%4s\t%4s\n",
		dx_to_name(this->label),
		byte_code_dex[this->op],
		result? dx_to_name(this->result->name) : dx_to_name(this->target),
		arg1,
		arg2
	);
}


icmd::icmd(
	name_dx      label,
	op_code      op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
){
	char arg1[20], arg2[20];
	char err_array[ERR_ARR_SZ];
	
	this->op = op;
	
	switch (op){
	case I_NOP : break;
	
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
			this->arg2_lit   = true;
			this->arg2.l = right->value;
			sprintf(arg2, "#%4llx", right->value);
		}
		else {
			this->arg2.s = right;
			sprintf(arg2, "%s", dx_to_name(right->name));
		}
		
	// Unaries
	case I_ASS :
	case I_REF :
	case I_DREF:
	case I_NEG :
	case I_NOT :
	case I_INV : // stuff for unaries and binaries
		if (left->type == st_lit_int){
			this->arg1_lit   = true;
			this->arg1.l = left->value;
			sprintf(arg1, "#%4llx", left->value);
		}
		else {
			this->arg1.s = left;
			sprintf(arg1, "%s", dx_to_name(left->name));
		}
		
		this->result = out;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left->type == st_lit_int){
			this->arg1_lit   = true;
			this->arg1.l = left->value;
			sprintf(arg1, "#%4llx", left->value);
		}
		else {
			this->arg1.s = left;
			sprintf(arg1, "%s", dx_to_name(left->name));
		}
		this->target = target;
		break;
		
	case I_BLK :
	case I_EBLK:
	case I_CALL:
	case I_RTRN:
	default:
		sprintf(err_array,
			"Internal: emit_this() called with cmd = %d",
			op
		);
		crit_error(err_array);
	}
	
	// queue up this operation
	DS_nq(global_inst_q, this);
	
	// Print to the text file if present
	if (debug_fd)
		fprintf(
			debug_fd,
			"%s\t%5s\t%5s\t%s\n",
			byte_code_dex[op],
			out  ? dx_to_name(out->name) : "",
			left ? arg1 : "",
			right? arg2 : ""
		);
}


/******************************************************************************/
//                                C WRAPPERS
/******************************************************************************/


void emit_lbl(name_dx lbl){
	emit_iop(I_NOP, lbl, NULL, NULL, NULL);
	if (debug_fd) fprintf(debug_fd, "\nlbl %s:", dx_to_name(lbl));
}

void emit_iop(
	op_code      op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
){
	new icmd(NO_NAME, op, target, out, left, right);
}

void Print_icmd(void * iop){}

void emit_lbl(name_dx lbl){}


