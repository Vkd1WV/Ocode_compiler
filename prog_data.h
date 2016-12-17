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
#include <stdlib.h>
#include <string.h>

//#include <data.h>
typedef struct _root* DS;

#include "errors.h"
#include "my_types.h"


/******************************* NAME ARRAY ***********************************/


typedef size_t str_dx; ///< indexes into the string_array


/********************************* SYMBOLS ************************************/


typedef enum{
	st_undef,
	st_int,      ///< an integer
	st_ref,      ///< a reference
	st_fun,      ///< a function
	st_sub,      ///< a subroutine
	st_lit_int,  ///< a literal integer to be inserted directly
	st_lit_str,  ///< a string literal
	st_type_def, ///< a type definition: struct or class
	st_cust,     ///< programmer defined types
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
	
	// Qualifiers
	bool stat;     // Is it a static data location
	bool constant; // should this data ever be changed again
	
	// Initialized and literal
	bool set;         // has this data location been set before
	bool init;        // is there a static initialization stored in `value`
	umax value;       // integer literals or initialized
	uint8_t * str;    // string literals and initialized arrays
	char * assembler; // contents of an asm fun or sub
	
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

#ifdef _GLOBALS_C
	const char * op_code_dex[NUM_I_CODES] = {
		"I_NOP" , "I_ASS", "I_REF" , "I_DREF", "I_NEG", "I_NOT" , "I_INV" ,
		"I_INC", "I_DEC" ,
		"I_MUL" , "I_DIV", "I_MOD" , "I_EXP", "I_LSH" , "I_RSH", "I_ADD" ,
		"I_SUB" ,
		"I_BAND", "I_BOR", "I_XOR" , "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  ,
		"I_LTE" ,
		"I_GTE" , "I_AND", "I_OR"  ,
		"I_JMP" , "I_JZ" , "I_PROC", "I_CALL", "I_RTRN"
	};
#else
	extern const char * op_code_dex[NUM_I_CODES];
#endif


/******************************************************************************/
//                            CLASS DEFINITION
/******************************************************************************/


class Program_data;

class Instruction_Queue{
	// DATA
private:
	DS q;
	Program_data * pd;
	
	
	// FUNCTIONS
public:
	 Instruction_Queue(Program_data * prog_data);
	~Instruction_Queue(void                    );
	
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
	
	void Print_iop(FILE * fd, iop_pt iop) const;
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
	// Used in symbols only
	static inline int cmp_sym(const void * left, const void * right){
		return strcmp( (char*)left, (char*)right );
	}
	static inline const void * sym_key(const void * symbol){
		return (const void *) (string_array+(sym_pt (symbol)->name));
	}
	
	// returns non-constant
	inline char * access_string(str_dx dx) const {
		if(dx == NO_NAME) return NULL;
		else return string_array+dx;
	}
	inline sym_pt find_sym(str_dx dx) const;
	
	void Print_sym (FILE * fd, sym_pt sym) const;
	
public:
	 Program_data(void);
	~Program_data(void);
	
	// Mutators
	str_dx              add_string(const char        * name);
	str_dx              new_label (void                    );
	sym_pt              new_var   (sym_type            type);
	void                remove_sym(str_dx              dx  );
	//Instruction_Queue * nq_blk    (Instruction_Queue * blk );
	
	// Accessors
	inline const char * get_string(str_dx dx) const {
		return (const char *) access_string(dx);
	}
	
	// Dump current state
	void Dump_sym(FILE * fd) const;
	void Dump_q  (FILE * fd) const { block_q.Dump(fd); };
};


#endif // _INTERMED_H


