/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _PROG_DEF_H
#define _PROG_DEF_H


/** Type definitions for the intermediate representation
*/

#include <data.h>
#include "my_types.h"

typedef DS * DS_pt;

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


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN const char * op_code_dex[NUM_I_CODES]
#ifdef _GLOBAL_C
	 = {
		"I_NOP" , "I_ASS", "I_REF" , "I_DREF", "I_NEG", "I_NOT" , "I_INV" ,
		"I_INC", "I_DEC" ,
		"I_MUL" , "I_DIV", "I_MOD" , "I_EXP", "I_LSH" , "I_RSH", "I_ADD" ,
		"I_SUB" ,
		"I_BAND", "I_BOR", "I_XOR" , "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  ,
		"I_LTE" ,
		"I_GTE" , "I_AND", "I_OR"  ,
		"I_JMP" , "I_JZ" , "I_PROC", "I_CALL", "I_RTRN"
	}
#endif
;

#undef EXTERN


#endif // _PROG_DEF_H


