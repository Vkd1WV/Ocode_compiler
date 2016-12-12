/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"


/******************************************************************************/
//                            GLOBAL CONSTANTS
/******************************************************************************/


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


/********************************** NAMES *************************************/

/// insert a new name into the name_array. returns its name_dx
name_dx add_name(char * name){
	static name_dx size;
	static name_dx next;
	       name_dx name_sz;
	       name_dx temporary;
	
	/* we have to keep track of things with an index to ensure that the data
	will continue to be accessable when the array is resized.
	*/
	
	// Initialize the name array
	if(!size){ // This should only run the first time
		name_array= (char*)malloc(sizeof(char) * NAME_ARR_SZ);
		if(!name_array) crit_error("Out of Memory");
		size = NAME_ARR_SZ;
	}
	
	name_sz = (name_dx)strlen(name)+1; // +1 for the null
	
	// resize the array if necessary
	if(size - next < name_sz){
		if ((unsigned long int)size * 2 > UINT_MAX )
			crit_error("Name_array maxed out");
		name_array = (char*) realloc(name_array, (size *= 2));
		if(!name_array) crit_error("Out of Memory");
	}
	
	// Add name
	strncpy(dx_to_name(next), name, name_sz);
	temporary = next;
	next += name_sz;
	return temporary;
}

// create and return a pointer to a unique label
name_dx new_label(void){
	static umax i;
	static char label[UNQ_NAME_SZ];
	// sufficiently large for 32-bit numbers in decimal and then some.
	
	
	sprintf(label, "_$%04lld", i++); // use $ to prevent collisions
	return add_name(label);
}

/**	create a new symbol table entry with a unique name.
 *	*	temporary symbol names all begin with %.
 */
sym_pt new_var(sym_type type){
	static umax i;
	char name[UNQ_NAME_SZ];
	static struct sym new_symbol; // initialized to 0
	
	// give it a unique name
	sprintf(name, "_#%04lld", i++);
	new_symbol.name = add_name(name);
	new_symbol.type = type;
	new_symbol.temp = true;
	
	if(new_symbol.stat || new_symbol.constant || new_symbol.init || new_symbol.size) puts("new_var() screwed up!!");
	
	// insert it into the symbol table and return it
	return (sym_pt)DS_insert(symbols, &new_symbol);
}

/*void emit_cmnt(const char* comment){*/
/*	if (debug_fd) fprintf(debug_fd, "\t# %s\n", comment);*/
/*}*/

void emit_iop(
	name_dx      label,
	op_code    op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
){
	icmd iop[1];
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
			if( DS_find(symbols, dx_to_name(right->name)) ) DS_remove(symbols);
			else err_msg("emit_iop(): Internal: couldn't find symbol");
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
			if( DS_find(symbols, dx_to_name(left->name)) ) DS_remove(symbols);
			else err_msg("emit_iop(): Internal: couldn't find symbol");
		}
		else iop->arg1.symbol = left;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left && left->type == st_lit_int){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			
			// remove the lit symbol
			if( DS_find(symbols, dx_to_name(left->name)) ) DS_remove(symbols);
			else err_msg("emit_iop(): Internal: couldn't find symbol");
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
	DS_nq(global_inst_q, iop);
	
	
	#ifdef IOP_ADDR
	sprintf(err_array, "iop saved at: %p", temp);
	debug_msg(err_array);
	#endif
	
}


