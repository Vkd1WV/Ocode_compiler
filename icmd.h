/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _ICMD_H
#define _ICMD_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


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
	I_BLK ,
	I_EBLK,
	I_CALL,
	I_RTRN,
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


/// string length limit for unique compiler generated labels
#define UNQ_NAME_SZ 16
#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array
#define NO_NAME     ((name_dx)UINT_MAX)
extern const char * op_code_dex[NUM_I_CODES];


/******************************************************************************/
//                             GLOBAL VARIABLES
/******************************************************************************/


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


EXTERN DS      symbols;       ///< symbol table
EXTERN DS      global_inst_q; ///< a global instruction queue
EXTERN DS      sub_inst_q;    ///< an instruction queue for subroutines
EXTERN char *  name_array;    ///< dynamic array for symbol and label names


#undef EXTERN


/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/


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

/********************** PRINT INTERMEDIATE REPRESENTATION *********************/

static inline void Print_icmd(FILE * fd, icmd * iop){
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
	
	if(!sym) puts("NULL");
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

/********************* DEBUG INTERMEDIATE REPRESENTATION **********************/

static inline void debug_sym(const char * message, sym_pt sym){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, yylineno);
		Print_sym(stderr, sym);
	}
}

static inline void debug_iop(const char * message, icmd * iop){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, yylineno);
		Print_icmd(stderr, iop);
	}
}


/******************************************************************************/
//                             GLOBAL PROTOTYPES
/******************************************************************************/


void Dump_symbols(FILE * fd);
void Dump_iq     (FILE * fd);

// Names
name_dx add_name(char * name);
name_dx     new_label(void); ///< get a new unique label
sym_pt new_var  (sym_type);  ///< Create a unique temporary symbol

// Emmiters
void emit_cmnt(const char* comment);
void emit_iop(
	name_dx      label,
	op_code      op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
);


#endif // _ICMD_H


