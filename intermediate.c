/*
 *	OMNI INTERMEDIATE CODE GENERATION
 *
 *	Omni Intermediate Code is an implementation of three-address-code. It
 *	consists of one command per line or structure. Normally, intermediate code
 *	is generated in a queue of structures. However, when the compiler's debug
 *	flag is set, the intermediate code emitters will also create text files
 *	reprsenting the contents of the interm_q.
 *
 *	The general format of Three Address Codes is:
 *		OP		result	arg1	arg2
 *		OP		result	arg1
 *	The code must also contain jump statements and labels for flow control:
 *		LBL		label
 *		JMP		label	[condition]
 *	There must be function calls
 *		PARAM	reg		val
 *		CALL	label
 *		RTRN
 *
 *	Every control structure is reimplemented in the Intermediate Code with
 *	labels and jump statements. This requires extra labels provided by the
 *	new_label() function.
 *
 *	Expressions are reimplemented as individual operations. These operations
 *	will eventually act directly on register values. In the Intermediate Code
 *	registers are represented as temp symbols. Temp symbol names all begin with
 *	the % symbol.
 *
 *	In the future we will be able to implement an optimizer that processes
 *	Intermediate code into better Intermediate Code.
 *
 */

#include "compiler.h"

/******************************************************************************/
//                             LOCAL PROTOTYPES
/******************************************************************************/


icmd * New_iop(void);


/******************************************************************************/
//                            PRIVATE FUNCTIONS
/******************************************************************************/


icmd * New_iop(void){
	icmd * new_op;
	
	new_op = calloc(1, sizeof(icmd));
	if (!new_op) crit_error("out of memory");
	
	if (nxt_lbl) {
		new_op->label = nxt_lbl;
		nxt_lbl = NULL;
	}
	
	return new_op;
}


/******************************************************************************/
//                    CREATE TEMPORARY LABELS AND SYMBOLS
/******************************************************************************/


// create and return a pointer to a unique label
const char* new_label(void){
	static umax i;
	static char label[UNQ_LABEL_SZ];
	
	sprintf(label, "__%04lld", i++); // use __ to prevent collisions
	return label;
}

/* create and return a pointer to a new symbol table entry. temporary symbol
 * names all begin with %.
 */
sym_entry* new_var(void){
	static umax i;
	
	// create a new symbol entry
	sym_entry* new_symbol=calloc(1, sizeof(sym_entry));
	if (!new_symbol) crit_error("Out of memory");
	
	// give it a unique name
	sprintf(new_symbol->name, "%%%04lld", i++);
	
	// insert it into the symbol table
	sort(global_symbols, new_symbol, new_symbol->name);
	
	// Copy stuff
	new_symbol->type = temp;
	
	// and return it
	return new_symbol;
}

void Initialize_intermediate(void){
	if(debug_fd) fprintf(debug_fd,"#Omnicode Intermidiate File\n");
	global_symbols=new_DS('l'); // initialize the symbol table
	interm_q      =new_DS('l'); // initialize the intermediate code queue
	*nxt_lbl      = 0         ; // make sure the nxt_lbl is empty
}

// Dump the symbol Table
void Dump_symbols(void){
	sym_pt sym;
	
	fprintf(debug_fd,"\n#Table\tType\tconst\tInit\tDref\n");
	
	pview(global_symbols, 0);
	while((sym=view_next(global_symbols))){
		if( sym->type != literal )
			fprintf(debug_fd, "%s:\t%d\t%d\t%d\t%p\n",
				sym->name,
				sym->type,
				sym->constant,
				sym->init,
				(void*) sym->dref
			);
	}
}


/******************************************************************************/
//                                EMITTERS
/******************************************************************************/


void emit_cmnt(const char* comment){
	if (debug_fd) fprintf(debug_fd, "\t# %s\n", comment);
}

void emit_lbl(char* lbl){
	if (nxt_lbl)
		crit_error("Internal Compiler Error: consecutive label declaration");
	// easiest way around this is to emmit null ops with different labels
	
	nxt_lbl = calloc(1, strlen(lbl)+1); // +1 for the null
	if (!nxt_lbl) crit_error("Out of Memory");
	
	strncpy(nxt_lbl, lbl, strlen(lbl)+1);
	if (debug_fd) fprintf(debug_fd, "\nlbl %s:", lbl);
}

/*void emit_triple(*/
/*	byte_code cmd,*/
/*	const sym_entry* out,*/
/*	const sym_entry* in*/
/*){*/
/*	char arg1[NAME_MAX];*/
/*	char err_array[ERR_ARR_SZ];*/
/*	*/
/*	switch (cmd){*/
/*	case I_NUL:*/
/*	case I_ASS:*/
/*	case I_REF:*/
/*	case I_DREF:*/
/*	case I_NEG:*/
/*	case I_NOT:*/
/*	case I_INV:*/
/*	default:*/
/*		sprintf(err_array,*/
/*			"Internal Compiler Error: emit_triple() called with cmd = %d",*/
/*			cmd*/
/*		);*/
/*		crit_error(err_array);*/
/*	}*/
/*	*/
/*	*/
/*	if (in->type == literal) sprintf(arg1, "#%4llx", in->value);*/
/*	else              sprintf(arg1, "%s", in->name);*/
/*	*/
/*	fprintf(outfile, "%s\t%s\t%s\n", cmd, out->name, arg1);*/
/*}*/

void emit_quad(
	byte_code op,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
){
	char arg1[NAME_MAX], arg2[NAME_MAX];
	char err_array[ERR_ARR_SZ];
	icmd * iop;
	
	iop = New_iop();
	iop->op = op;
	
	switch (op){
	case I_NUL : break;
	
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
		if (right->type == literal){
			iop->arg2_lit   = true;
			iop->arg2.value = right->value;
			sprintf(arg2, "#%4llx", right->value);
		}
		else {
			iop->arg2.symbol = right;
			sprintf(arg2, "%s", right->name);
		}
		
	// Unaries
	case I_ASS :
	case I_REF :
	case I_DREF:
	case I_NEG :
	case I_NOT :
	case I_INV : // stuff for unaries and binaries
		if (left->type == literal){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			sprintf(arg1, "#%4llx", left->value);
		}
		else {
			iop->arg1.symbol = left;
			sprintf(arg1, "%s", left->name);
		}
		
		iop->result = out;
		break;
		
	case I_JMP :
	case I_JZ  :
	case I_BLK :
	case I_EBLK:
	case I_CALL:
	case I_RTRN:
	default:
		sprintf(err_array,
			"Internal Compiler Error: emit_triple() called with cmd = %d",
			op
		);
		crit_error(err_array);
	}
	
	// queue up this operation
	nq(interm_q, iop);
	
	// Print to the text file if present
	if (debug_fd)
		fprintf(debug_fd, "%d\t%s\t%s\t%s\n", op, out->name, arg1, arg2);
}





