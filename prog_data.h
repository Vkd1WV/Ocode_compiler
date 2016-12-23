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


#include <stdio.h>

//#include <data.h>
typedef struct _root* DS;

#include "my_types.h"


/******************************* NAME ARRAY ***********************************/


typedef size_t str_dx; ///< indexes into the string_array


/********************************* SYMBOLS ************************************/

/* 

*/


typedef enum{
	st_undef,
	// Storage
	st_int,      ///< an integer
	st_ref,      ///< a reference
	
	st_void,     ///< explicitly nothing
	st_sub,      ///< a subroutine
	st_lit_int,  ///< a literal integer to be inserted directly
	st_lit_str,  ///< a string literal
	st_rec_def,   ///< a type definition: struct or class
	st_rec,
	st_union_def,
	st_union,
	st_alias,    ///< an alias for another type
	st_NUM
} sym_type;

typedef enum{
	w_undef,
	w_byte,  ///< an 8-bit integer
	w_byte2, ///< a 16-bit integer
	w_word,  ///< an integer of the natural width of the target processor
	w_byte4, ///< a 32-bit integer
	w_max,   ///< an integer of the greatest width of the target processor
	w_byte8, ///< a 64-bit integer
	w_NUM
} int_size;

typedef enum{
	mt_undef,
	mt_in,    ///< read only parameter
	mt_out,   ///< write only parameter
	mt_bi,    ///< bi-directional parameter
	mt_rtrn,  ///< return value
	mt_pub,   ///< a public member
	mt_priv,  ///< a private member
	mt_NUM
} member_type;

typedef struct sym {
	str_dx  name; ///< index into the name_array
	str_dx  short_name;
	sym_type type; ///< Symbol type
	bool     temp;
	
	// function
	bool fun;
	
	// Qualifiers
	bool stat;     // Is it a static data location
	bool constant; // should this data ever be changed again
	
	// Initialized and literal
	bool set;         // has this data location been set before
	bool init;        // is there a static initialization stored in `value`
	umax value;       // integer literals or initialized
	str_dx str;       // string literals and initialized arrays
	const char * assembler; // contents of an asm fun or sub
	
	// Reference
	struct sym* dref;
	
	// Integers
	int_size size;
	
	// procedures, typedefs, and customs
	DS members; // parameters of procedures, members of typedefs and customs
	member_type mt; // if a parameter/member, what type
	size_t offset;  // SP offset for automatic variables
	
	// Used by the optomizer
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
	I_PARM,
	I_CALL,
	I_PROC, // first icmd of each procedure. sets auto variables
	I_RTRN,
	
	NUM_I_CODES
}op_code;

typedef union {
	sym_pt symbol; ///< a variable
	umax   value;  ///< a literal
} intermed_arg;


typedef struct iop {
	str_dx      label;    ///< The label, if any, for this operation
	str_dx      target;   ///< target of a jump

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
} * iop_pt;


/******************************************************************************/
//                            GLOBAL CONSTANTS
/******************************************************************************/


#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array
#define NO_NAME     ((str_dx)UINT_MAX)

#define START_LBL "_#START"

#ifdef _GLOBAL_C
	const char * op_code_dex[NUM_I_CODES] = {
		"I_NOP" , "I_ASS", "I_REF" , "I_DREF", "I_NEG", "I_NOT" , "I_INV" ,
		"I_INC", "I_DEC" ,
		"I_MUL" , "I_DIV", "I_MOD" , "I_EXP", "I_LSH" , "I_RSH", "I_ADD" ,
		"I_SUB" ,
		"I_BAND", "I_BOR", "I_XOR" , "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  ,
		"I_LTE" ,
		"I_GTE" , "I_AND", "I_OR"  ,
		"I_JMP" , "I_JZ" ,
		"I_PARM", "I_CALL", "I_PROC", "I_RTRN"
	};
#else
	extern const char * op_code_dex[NUM_I_CODES];
#endif


/******************************************************************************/
//                            CLASS DEFINITION
/******************************************************************************/


class Instruction_Queue{
	// DATA
private:
	DS q;
	
	// FUNCTIONS
public:
	 Instruction_Queue(void);
	~Instruction_Queue(void);
	
	bool   isempty (void) const;
	uint   count   (void) const;
	void   flush   (void)      ;
	
	iop_pt dq      (void)      ;
	iop_pt nq      (iop_pt)    ;
	iop_pt remove  (void)      ;
	
	void add_inst(
		str_dx      label,
		op_code    op,
		str_dx      target,
		const sym_pt out,
		const sym_pt left,
		const sym_pt right
	);
	
	iop_pt first   (void) const;
	iop_pt last    (void) const;
	iop_pt next    (void) const;
	iop_pt previous(void) const;
	
	void Dump     (FILE * fd            ) const;
};


class Block_Queue{
private:
	DS bq;
public:
	 Block_Queue(void);
	~Block_Queue(void);
	
	bool                isempty(void                 ) const;
	uint                count  (void                 ) const;
	Instruction_Queue * first  (void                 ) const;
	Instruction_Queue * last   (void                 ) const;
	Instruction_Queue * next   (void                 ) const;
	Instruction_Queue * nq     (Instruction_Queue * q)      ;
	Instruction_Queue * dq     (void                 )      ;
	void                Dump   (FILE * fd            ) const;
};


// All program data is stored here
class Program_data{
private:
	// DATA
	// These are static because only one Program_data can exist at a time
	static char *      string_array;
	static str_dx      sa_size;
	static str_dx      sa_next;
	
	static DS          symbols;
public:
	static Block_Queue block_q;
	
	// FUNCTIONS
private:
	
	
	// returns non-constant
	static inline char * access_string(str_dx dx){
		if(dx == NO_NAME) return NULL;
		else return string_array+dx;
	}
	
	static const char * unique_str(void);
	
public:
	 Program_data(void);
	~Program_data(void);
	
	// Mutators
	static str_dx add_string(const char * name  );
	static void   add_sym   (sym_pt     & symbol);
	
	static str_dx unq_label (void               );
	static sym_pt unq_sym   (sym_type     type  );
	//static sym_pt dup_sym   (sym_pt symbol      );
	
	static void   remove_sym(str_dx       dx    );
	
	static sym_pt find_sym(const char * name);
	
	// Accessors
	static inline const char * get_string(str_dx dx) {
		if(dx == NO_NAME) return NULL;
		else return string_array+dx;
	}
	
	// Dump current state
	void Dump_sym(FILE * fd) const;
	void Dump(FILE * fd) const {
		fputs("\nSYMBOLS\n", fd);
		Dump_sym(fd);
		fputs("\nBLOCK QUEUE\n", fd);
		block_q.Dump(fd);
	}
};


/******************************************************************************/
//                            INLINE FUNCTIONS
/******************************************************************************/


static inline void Print_iop(FILE * fd, iop_pt iop) {
	const char *result, *arg1, *arg2;
	const char *none = "NONE", *lit = "lit";
	
	if(!iop) fputs("Print_icmd(): NULL\n", fd);
	else{
		if(iop->target != NO_NAME)
			result = Program_data::get_string(iop->target);
		else if(iop->result)
			result = Program_data::get_string(iop->result->name);
		else result = none;
		
		if(iop->arg1_lit) arg1 = lit;
		else if (iop->arg1.symbol)
			arg1 = Program_data::get_string(iop->arg1.symbol->name);
		else arg1 = none;
		
		if(iop->arg2_lit) arg2 = lit;
		else if (iop->arg2.symbol)
			arg2 = Program_data::get_string(iop->arg2.symbol->name);
		else arg2 = none;
		
		fprintf(fd, "%6s:\t%s\t%c %6s\t%c %6s\t%c %6s\n",
			Program_data::get_string(iop->label),
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

static inline void Print_sym(FILE * fd, sym_pt sym) {
	const char * types[st_NUM] = {
		"undef", "int", "ref", "fun", "sub", "lit_int", "lit_str", "tp_def"
	};
	const char * widths[w_NUM] = {
		"undef", "byte", "byte2", "word", "byte4", "max", "byte8"
	};
	
	if(!sym) fputs("NULL\n", fd);
	else
		fprintf(fd, "%6s:\t%7s %5s %s%s%s%s%s  %p\n",
			Program_data::get_string(sym->name),
			types[sym->type],
			sym->type == st_int? widths[sym->size] : "",
			sym->temp    ? "t": " ",
			sym->constant? "c": " ",
			sym->stat    ? "s": " ",
			sym->set     ? "v": " ", // whether it currently has a value assigned
			sym->init    ? "i": " ", // wether it has a static initialization
			(void*) sym->dref
		);
}


#endif // _INTERMED_H


