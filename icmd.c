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


const char * op_code_dex[NUM_I_CODES] = {
	"I_NOP", "I_ASS", "I_REF", "I_DREF", "I_NEG", "I_NOT" , "I_INV" , "I_INC",
	"I_DEC",
	"I_MUL",
	"I_DIV", "I_MOD", "I_EXP", "I_LSH" , "I_RSH", "I_ADD" , "I_SUB" , "I_BAND",
	"I_BOR", "I_XOR", "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  , "I_LTE" , "I_GTE" ,
	"I_AND", "I_OR" , "I_JMP", "I_JZ"  , "I_BLK", "I_EBLK", "I_CALL", "I_RTRN"
};


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


// Dump the symbol Table
void Dump_symbols(FILE * fd){
	sym_pt sym;
	
	info_msg("Dumping Symbols");
	fputs("# SYMBOL TABLE", fd);
	fprintf(fd,"\nName:\t   Type Width Flags Dref\n");
	
	sym = (sym_pt) DS_first(symbols);
	do {
		Print_sym(fd, sym);
	} while(( sym = (sym_pt) DS_next(symbols) ));
	
	fputs("\n\n", fd);
	
	fflush(fd);
	
	info_msg("Finished Symbols");
}

void Dump_iq(FILE * fd){
	icmd * iop;
	
	info_msg("Dumping the Global queue");
	fprintf(fd, "GLOBAL QUEUE\n");
	fprintf(fd, "LBL:\tI_OP\tRESULT\tARG1\tARG2\n");
	
	if(verbosity >= V_DEBUG) fflush(fd);
	
	iop = (icmd*) DS_first(global_inst_q);
	
	do {
		sprintf(err_array, "Printing iop at: %p", (void*)iop);
		debug_msg(err_array);
		
		Print_icmd(fd, iop);
		if(verbosity >= V_DEBUG) fflush(fd);
	} while (( iop = (icmd*) DS_next(global_inst_q) ));
	
	fputs("\n\n", fd);
	fflush(fd);
	
	info_msg("Dumping the sub queue");
	fprintf(fd, "SUB QUEUE\n");
	fprintf(fd, "LBL:\tI_OP\tRESULT\tARG1\tARG2\n");
	
	fflush(fd);
	
	iop = (icmd*) DS_first(sub_inst_q);
	do {
		Print_icmd(fd, iop);
	} while (( iop = (icmd*) DS_next(sub_inst_q) ));
	
	fflush(fd);
}

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
	
	name_sz = (name_dx)strlen(name) + 1; // +1 for the null
	
	// resize the array if necessary
	if(size - next < name_sz){
		if ((unsigned long int)size * 2 > UINT_MAX )
			crit_error("Name_array maxed out");
		name_array = (char*) realloc(name_array, (size *= 2));
		if(!name_array) crit_error("Out of Memory");
	}
	
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


/******************************** EMITTERS ************************************/

void emit_cmnt(const char* comment){
	if (debug_fd) fprintf(debug_fd, "\t# %s\n", comment);
}

void emit_iop(
	name_dx      label,
	op_code    op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
){
	icmd iop[1];
	void * temp;
	
	iop->label  = label;
	iop->op     = op;
	iop->target = target;
	
	debug_sym("emit_iop()        : out         ", out);
	debug_sym("emit_iop()        : left        ", left);
	debug_sym("emit_iop()        : right       ", right);
	
	switch (op){
	case I_NOP :
	case I_RTRN: break;
	
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
		}
		else iop->arg1.symbol = left;
		
		iop->result = out;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left->type == st_lit_int){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
		}
		else iop->arg1.symbol = left;
		break;
		
	case I_INC :
	case I_DEC :
	case I_BLK :
	case I_EBLK:
	case I_CALL:
	
	case NUM_I_CODES:
	default:
		sprintf(err_array,
			"Internal: emit_iop() called with cmd = %d",
			op
		);
		crit_error(err_array);
	}
	
	debug_iop("emit_iop()        :Adding iop   ", iop);
	
	// queue up this operation
	temp= DS_nq(global_inst_q, iop);
	sprintf(err_array, "iop saved at: %p", temp);
	debug_msg(err_array);
	
}


