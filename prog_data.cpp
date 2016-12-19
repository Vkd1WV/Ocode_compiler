/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/ 


#include "prog_data.h"
#include "errors.h"
#include <data.h> // put this here to force usage of wrappers
#include <string.h>
#include <stdlib.h>


/******************************************************************************/
//                           PROGRAM DATA CLASS
/******************************************************************************/


const char colision_char = '#';

/******************************* Static Data **********************************/

char * Program_data::string_array;
str_dx Program_data::sa_size;
str_dx Program_data::sa_next;
DS     Program_data::symbols;

/**************************** Member Functions ********************************/

// Used in symbols only
static inline int cmp_sym(const void * left, const void * right){
	return strcmp( (char*)left, (char*)right );
}
static inline const void * sym_key(const void * symbol){
	return (const void *) Program_data::get_string(sym_pt (symbol)->name);
}

Program_data::Program_data(void){
	// Initialize the string array
	string_array = (char*)malloc(sizeof(char) * NAME_ARR_SZ);
	if(!string_array) crit_error("Out of Memory");
	sa_size = NAME_ARR_SZ;
	
	symbols = (DS) DS_new_bst(
		sizeof(struct sym),
		false,
		&sym_key,
		&cmp_sym
	);
}

Program_data::~Program_data(void){
	debug_msg("Program_data::~Program_data(): start");
	
	debug_msg("Deleting the name array");
	free(string_array);
	
	debug_msg("Deleting the symbol table");
	DS_delete(symbols);
	
	debug_msg("Program_data::~Program_data(): stop");
}


str_dx Program_data::add_string(const char * name){
	str_dx name_sz;
	str_dx temporary;
	
	/* we have to keep track of things with an index to ensure that the data
	will continue to be accessable when the array is resized.
	*/
	
	name_sz = (str_dx)strlen(name)+1; // +1 for the null
	
	// resize the array if necessary
	if(sa_size - sa_next < name_sz){
		if ((unsigned long int)sa_size * 2 > UINT_MAX )
			crit_error("Name_array maxed out");
		string_array = (char*) realloc(string_array, (sa_size *= 2));
		if(!string_array) crit_error("Out of Memory");
	}
	
	// Add name
	strncpy(access_string(sa_next), name, name_sz);
	temporary = sa_next;
	sa_next += name_sz;
	return temporary;
}

const char * Program_data::unique_str(void){
	static umax i;
	static char str[23];
	//2^64 = 10^x -> log_10 2^64 = x -> x=19.2659...
	// +3
	
	if(i == UMAX_MAX) crit_error("Program_data::unique_str(): maxed out");
	
	sprintf(str, "_%c%04lld", colision_char, i++);
	return str;
}

str_dx Program_data::new_label(void){
	return add_string(unique_str());
}


/**	create a new symbol table entry with a unique name.
 *	*	temporary symbol names all begin with %.
 */
sym_pt Program_data::new_var(sym_type type){
	static struct sym new_symbol; // initialized to 0
	
	// give it a unique name
	new_symbol.name = add_string(unique_str());
	new_symbol.type = type;
	new_symbol.temp = true;
	
	if(new_symbol.stat || new_symbol.constant || new_symbol.init || new_symbol.size) puts("new_var() screwed up!!");
	
	// insert it into the symbol table and return it
	return (sym_pt)DS_insert(symbols, &new_symbol);
}


inline sym_pt Program_data::find_sym(str_dx dx) const {
	return (sym_pt)DS_find(symbols, get_string(dx));
}
void Program_data::remove_sym(str_dx dx){
	if( DS_find(symbols, get_string(dx)) ) DS_remove(symbols);
	else err_msg("Program_data::remove_sym(): Internal: couldn't find symbol");
}


// Dump the symbol Table
void Program_data::Dump_sym(FILE * fd) const {
	sym_pt sym;
	
	debug_msg("\tDump_symbols(): start");
	if (!fd) err_msg("Internal: Dump_symbols(): received NULL file descriptor");
	
	if (DS_count(symbols)){
		fputs("  Name:\t   Type Width Flags Dref\n", fd);
		
		sym = (sym_pt) DS_first(symbols);
		do {
			Print_sym(fd, sym);
		} while(( sym = (sym_pt) DS_next(symbols) ));
		
		#ifdef FLUSH_FILES
			fflush(fd);
		#endif
	}
	else{
		info_msg("\tDump_symbols(): The Symbol list is empty");
		fputs("Empty\n", fd);
	}
	
	debug_msg("\tDump_symbols(): stop");
}


/******************************************************************************/
//                            BLOCK QUEUE CLASS
/******************************************************************************/


/* We don't want to actually copy the incoming instruction queues. We just want to keep a reference.
*/

Block_Queue::Block_Queue(void) {
	bq = (DS) DS_new_list(sizeof(Instruction_Queue*));
}

Block_Queue::~Block_Queue(void) {
	Instruction_Queue * iq;
	
	debug_msg("Block_Queue::~Block_Queue(): start");
	
	while(( iq = dq() )) delete iq;
	
	DS_delete(bq);
	
	debug_msg("Block_Queue::~Block_Queue(): stop");
}

bool Block_Queue::isempty(void) const { return DS_isempty(bq); }
uint Block_Queue::count  (void) const { return DS_count  (bq); }

// FIXME: any of these could return NULL
Instruction_Queue * Block_Queue::first(void) const {
	Instruction_Queue** pt;
	
	pt = (Instruction_Queue**) DS_first(bq);
	if(pt) return *pt;
	else return NULL;
}
Instruction_Queue * Block_Queue::last(void) const{
	Instruction_Queue** pt;
	
	pt = (Instruction_Queue**) DS_last(bq);
	if(pt) return *pt;
	else return NULL;
}
Instruction_Queue * Block_Queue::next(void) const {
	Instruction_Queue** pt;
	
	pt = (Instruction_Queue**) DS_next(bq);
	if(pt) return *pt;
	else return NULL;
}
Instruction_Queue * Block_Queue::nq(Instruction_Queue * q){
	return *(Instruction_Queue**) DS_nq(bq, &q);
}
Instruction_Queue * Block_Queue::dq(void){
	Instruction_Queue** pt;
	
	pt = (Instruction_Queue**) DS_dq(bq);
	if(pt) return *pt;
	else return NULL;
}

void Block_Queue::Dump(FILE * fd) const {
	Instruction_Queue * blk_pt;
	
	debug_msg("\tBlock_Queue::Dump(): start");
	if (!fd)
		err_msg("Internal: Block_Queue::Dump(): received NULL file descriptor");
	
	if(!isempty()){
		blk_pt = first();
		fputs("LBL   :\tI_OP\t RESULT \t  ARG1  \t  ARG2", fd);
		do {
			if(!blk_pt) err_msg("\tBlock_Queue::Dump(): found an empty block");
			
			else{
				fputs("\n", fd);
				blk_pt->Dump(fd);
			}
		} while(( blk_pt = next() ));
	}
	else {
		info_msg("\tBlock_Queue::Dump(): The block queue is empty");
		fputs("Empty\n", fd);
	}
	
	debug_msg("\tBlock_Queue::Dump(): stop");
}


/******************************************************************************/
//                         INSTRUCTION QUEUE CLASS
/******************************************************************************/




Instruction_Queue::Instruction_Queue(void) {
	q = DS_new_list(sizeof(struct iop));
}
Instruction_Queue::~Instruction_Queue(void)  { DS_delete(q); }

// Wrapper functions
bool   Instruction_Queue::isempty (void) const{ return         DS_isempty (q); }
uint   Instruction_Queue::count   (void) const{ return         DS_count   (q); }
void   Instruction_Queue::flush   (void)      {                DS_flush   (q); }
iop_pt Instruction_Queue::first   (void) const{ return(iop_pt) DS_first   (q); }
iop_pt Instruction_Queue::last    (void) const{ return(iop_pt) DS_last    (q); }
iop_pt Instruction_Queue::next    (void) const{ return(iop_pt) DS_next    (q); }
iop_pt Instruction_Queue::previous(void) const{ return(iop_pt) DS_previous(q); }
iop_pt Instruction_Queue::dq      (void)      { return(iop_pt) DS_dq      (q); }
iop_pt Instruction_Queue::nq      (iop_pt i)  { return(iop_pt) DS_nq    (q,i); }
iop_pt Instruction_Queue::remove  (void)      { return(iop_pt) DS_remove  (q); }

void Instruction_Queue::add_inst(
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
			Program_data::remove_sym(right->name);
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
			Program_data::remove_sym(left->name);
		}
		else iop->arg1.symbol = left;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left && left->type == st_lit_int){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			
			// remove the lit symbol
			Program_data::remove_sym(left->name);
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

void Instruction_Queue::Dump(FILE * fd) const {
	iop_pt iop;
	
	//debug_msg("\tDump_iq(): start");
	if (!fd) err_msg("Internal: Dump_iq(): received NULL file descriptor");
	
	#ifdef FLUSH_FILES
		fflush(fd);
	#endif
	
	if( !DS_isempty(q) ){
		iop = first();
		do {
			#ifdef IOP_ADDR
			sprintf(err_array, "Printing iop at: %p", (void*)iop);
			debug_msg(err_array);
			#endif
		
			Print_iop(fd, iop);
			
			#ifdef FLUSH_FILES
				fflush(fd);
			#endif
		} while (( iop = next() ));
	}
	else {
		info_msg("\tDump_iq(): The queue is empty");
		fputs("Empty\n", fd);
	}
	
	//debug_msg("\tDump_iq(): stop");
}


