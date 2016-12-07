/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _INTERMED_H
#define _INTERMED_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


// All program data is stored here
typedef struct {
	char * names;
	DS     block_q;
	DS     main_q;
	DS     sub_q;
	DS     symbols;
} Program_data;

/******************************* NAME ARRAY ***********************************/

typedef unsigned int name_dx; ///< indexes into the name_array

/********************************* SYMBOLS ************************************/

typedef enum {
	st_undef,
	st_int,      ///< an integer
	st_ref,      ///< a reference
	st_fun,      ///< a function
	st_sub,      ///< a subroutine
	st_lit_int,  ///< a literal integer to be inserted directly
	st_lit_str,  ///< a string literal
	st_type_def, ///< defined type or struct or class
	st_NUM
} sym_type;

typedef enum {
	w_undef,
	w_word,  ///< an integer of the natural width of the target processor
	w_max,   ///< an integer of the greatest width of the target processor
	w_byte,  ///< an 8-bit integer
	w_byte2, ///< a 16-bit integer
	w_byte4, ///< a 32-bit integer
	w_byte8, ///< a 64-bit integer
	w_NUM
} int_size;

typedef struct sym {
	name_dx  name; ///< index into the name_array
	sym_type type; ///< Symbol type
	bool     temp;
	
	// Qualifiers
	bool stat;     // Is it a static data location
	bool constant; // should this data ever be changed again
	
	// Initialized and literal
	bool init;        // Is the data location initialized
	umax value;       // integer literals or initialized
	uint8_t * str;    // string literals and initialized arrays
	char * assembler; // contents of an asm fun or sub
	
	// Reference
	struct sym* dref;
	
	// Integers
	int_size size;
	
	// function and subroutine
	//DS   local;     // Local scope for functions and structures
	// Parameter specification
	// return value
	
	// Used by the code generators
	bool live;
} * sym_pt;

/************************* INTERMEDIATE INSTRUCTIONS **************************/

typedef enum {
	I_NOP,

	// Unary OPS (8)
	I_ASS ,
	I_REF ,
	I_DREF,
	I_NEG ,
	I_NOT ,
	I_INV ,
	I_INC ,
	I_DEC ,

	// Binary OPS (19)
	I_MUL,
	I_DIV,
	I_MOD,
	I_EXP,
	I_LSH,
	I_RSH,

	I_ADD ,
	I_SUB ,
	I_BAND,
	I_BOR ,
	I_XOR ,

	I_EQ ,
	I_NEQ,
	I_LT ,
	I_GT ,
	I_LTE,
	I_GTE,

	I_AND,
	I_OR ,

	// Flow Control (6)
	I_JMP ,
	I_JZ  ,
	I_CALL,
	I_RTRN,
	
	// Other
	I_CMNT,
	I_BLK ,
	I_EBLK,
	NUM_I_CODES
}op_code;

typedef union {
	sym_pt symbol; ///< a variable
	umax   value;  ///< a literal
} intermed_arg;


typedef struct icode {
	name_dx      label;    ///< The label, if any, for this operation
	name_dx      target;   ///< target of a jump

	sym_pt       result;   ///< result of the operation
	intermed_arg arg1;     ///< first argument
	intermed_arg arg2;     ///< second argument
	bool         arg1_lit; ///< whether arg1 is literal or a symbol
	bool         arg2_lit; ///< whether arg2 is literal or a symbol
	
	op_code    op;       ///< The intermediate operator
	
	// Used by code generators
	bool result_live;
	bool arg1_live;
	bool arg2_live;
} icmd;


/******************************************************************************/
//                            GLOBAL CONSTANTS
/******************************************************************************/


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/// string length limit for unique compiler generated labels
#define UNQ_NAME_SZ 16
#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array
#define NO_NAME     ((name_dx)UINT_MAX)

EXTERN const char * op_code_dex[NUM_I_CODES]
#ifdef _GLOBALS_C
= {
	"I_NOP" , "I_ASS", "I_REF" , "I_DREF", "I_NEG", "I_NOT" , "I_INV" , "I_INC",
	"I_DEC" ,
	"I_MUL" , "I_DIV", "I_MOD" , "I_EXP", "I_LSH" , "I_RSH", "I_ADD" , "I_SUB" ,
	"I_BAND", "I_BOR", "I_XOR" , "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  , "I_LTE" ,
	"I_GTE" , "I_AND", "I_OR"  ,
	"I_JMP" , "I_JZ" , "I_CALL", "I_RTRN",
	"I_CMNT", "I_BLK", "I_EBLK"
}
#endif // _GLOBALS_C
;

#undef EXTERN

/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/

extern char * name_array;

/// find a name in the name_array by its name_dx
static inline char * dx_to_name(name_dx index){
	if(index == NO_NAME) return NULL;
	else return name_array+index;
}

static inline int cmp_sym(const void * left, const void * right){
	return strcmp( (char*)left, (char*)right );
}

static inline const void * sym_key(const void * symbol){
	return dx_to_name(((sym_pt)symbol)->name);
}

static inline void Init_program_data(Program_data * data_pt){
	data_pt->block_q = (DS) DS_new_list(sizeof(DS));
	data_pt->main_q  = (DS) DS_new_list(sizeof(icmd));
	data_pt->sub_q   = (DS) DS_new_list(sizeof(icmd));
	data_pt->symbols = (DS) DS_new_bst(
		sizeof(struct sym),
		false,
		&sym_key,
		&cmp_sym
	);
}

static inline void Clear_program_data(Program_data * data_pt){
	DS blk;

	debug_msg("Deleting the name array");
	free(data_pt->names);
	
	debug_msg("Deleting the blocks");
	while(( blk = (DS) DS_first(data_pt->block_q) )) DS_delete(blk);
	debug_msg("Deleting the block queue");
	DS_delete(data_pt->block_q);
	
	debug_msg("Deleting the main Queue");
	DS_delete(data_pt->main_q);
	debug_msg("Deleting the Sub queue");
	DS_delete(data_pt->sub_q);
	debug_msg("Deleting the symbol table");
	DS_delete(data_pt->symbols);
}

/********************** PRINT INTERMEDIATE REPRESENTATION *********************/

static inline void Print_icmd(FILE * fd, icmd * iop){
	if(!iop) fputs("NULL\n", fd);
	else
		fprintf(fd, "%4s:\t%s\t%4s\n",
			dx_to_name(iop->label),
			op_code_dex[iop->op],
			dx_to_name(iop->target)
		);
}

static inline void Print_sym(FILE * fd, sym_pt sym){
	char * types[st_NUM] = {
		"undef", "int", "ref", "fun", "sub", "lit_int", "lit_str", "tp_def"
	};
	char * widths[w_NUM] = {
		"undef", "word", "max", "1", "2", "4", "8"
	};
	
	if(!sym) fputs("NULL\n", fd);
	else
		fprintf(fd, "%4s:\t%7s %5s %s%s%s%s  %p\n",
			dx_to_name(sym->name),
			types[sym->type],
			sym->type == st_int? widths[sym->size] : "",
			sym->temp    ? "t": " ",
			sym->constant? "c": " ",
			sym->stat    ? "s": " ",
			sym->init    ? "i": " ",
			(void*) sym->dref
		);
}

/**************** DUMP INTERMEDIATE REPRESENTATION TO A FILE ******************/

static inline void Dump_iq(FILE * fd, DS q){
	icmd * iop;
	
	if (!fd) err_msg("Internal: Dump_iq(): received NULL file descriptor");
	if (!q) err_msg("Internal: Dump_iq(): received NULL queue");
	
	if(verbosity >= V_DEBUG) fflush(fd);
	
	fprintf(fd, "LBL:\tI_OP\tRESULT\tARG1\tARG2\n");
	
	iop = (icmd*) DS_first(q);
	
	do {
		#ifdef IOP_ADDR
		sprintf(err_array, "Printing iop at: %p", (void*)iop);
		debug_msg(err_array);
		#endif
		
		Print_icmd(fd, iop);
		if(verbosity >= V_DEBUG) fflush(fd);
	} while (( iop = (icmd*) DS_next(q) ));
}

static inline void Dump_blkq(FILE * fd, Program_data * data){
	DS blk;
	
	if (!fd) err_msg("Internal: Dump_blkq(): received NULL file descriptor");
	if (!data->block_q)
		err_msg("Internal: Dump_blkq(): received NULL block queue");
	
	info_msg("Dumping the block queue");
	
	blk = (DS) DS_first(data->block_q);
	
	do {
		Dump_iq(fd, blk);
	} while(( blk = (DS) DS_next(data->block_q) ));
	
	info_msg("Finished dumping the block queue");
}

// Dump the symbol Table
static inline void Dump_symbols(FILE * fd, Program_data * data){
	sym_pt sym;
	
	if (!fd) err_msg("Internal: Dump_symbols(): received NULL file descriptor");
	if (!data->symbols)
		err_msg("Internal: Dump_symbols(): received NULL symbols");
	
	info_msg("Dumping Symbols...");
	fputs("# SYMBOL TABLE", fd);
	fprintf(fd,"\nName:\t   Type Width Flags Dref\n");
	
	sym = (sym_pt) DS_first(data->symbols);
	do {
		Print_sym(fd, sym);
	} while(( sym = (sym_pt) DS_next(data->symbols) ));
	
	fputs("\n\n", fd);
	
	fflush(fd);
	
	info_msg("Finished Symbols");
}

static inline void Dump_all(FILE * fd, Program_data * data){
	info_msg("Dumping all program data");
	
	sprintf(err_array, "The symbols has %u", DS_count(data->symbols));
	info_msg(err_array);
	Dump_symbols(fd, data);
	
	sprintf(err_array, "The main_q has %u instructions", DS_count(data->main_q));
	info_msg(err_array);
	fprintf(fd, "MAIN QUEUE\n");
	Dump_iq(fd, data->main_q);
	
	fputs("\n\n", fd);
	fflush(fd);
	
	sprintf(err_array, "The sub_q has %u instructions", DS_count(data->sub_q));
	info_msg(err_array);
	fprintf(fd, "SUB QUEUE\n");
	Dump_iq(fd, data->sub_q);
	
	fputs("\n\n", fd);
	fflush(fd);
	
	sprintf(err_array, "The block_q has %u blocks", DS_count(data->block_q));
	info_msg(err_array);
	fprintf(fd, "BLOCK QUEUE\n");
	Dump_blkq(fd, data);
	
	fflush(fd);
	
	info_msg("Finished dumping program data");
}


#endif // _INTERMED_H


