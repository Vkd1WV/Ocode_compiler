/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _TYPES_H
#define _TYPES_H

/******************************************************************************/
//                            Type Definitions
/******************************************************************************/


typedef unsigned long long umax;
typedef unsigned int uint;
typedef unsigned int name_dx; ///< indexes into the name_array

typedef uint16_t token_t;

//typedef enum {Real, Protected, Virtual, SMM, Compatibility, Long} mode_t;
//typedef enum {x86, arm} arch_t;



/********************************* SYMBOLS ************************************/
// Each symbol table entry contains all data related to named symbols in the 
// program. Some symbols reprsent an address location. The symbol table entry
// indicates what is at that location.
// Other symbols like typedefs or constants only exist at compile time.


/*	Variable
		assume zero initialization
		insert symbol when used
	Initialized variable
		insert symbol when used
	declared const
		insert value when used
	immediate constant
		insert value when used
*/

/*	SYMBOLS
RUN-TIME
	Variable
		Datatype
			size of the memory location
		Initial value
		static or dynamic memory
		whether assignments are allowed in the current context
	Subroutine
		parameter specification
		local symbols
	Function
		parameter specification
		local symbols
		return datatype
	pointer
		how many derefs
		datatype pointed to
		static or dynamic memory
		whether assignments are allowed and at which level of dereference
COMPILE-TIME
	Literal values and declared constants
		Datatype
		value
		Just insert the value when used
	Type Definition
		??
*/

typedef enum {
	void_t,	// default value if unassigned
	word,	// natural unit for given processor
	byte,	// 8 bits
	byte2,	// 16 bits
	byte3,	// 24 bits
	byte4,	// 32 bits
	byte8,	// 64 bits
	max_t	// the largest unit for given processor
} width_t;

typedef enum {
	none,       // default value if unassigned
	temp,       // a place holder for a register
	data,
	pointer,    // a pointer to the symbol contained in dref
	function,
	subroutine,
	literal,    // a literal number to be inserted directly
	type_def    // defined type
} symbol_t;

//switch (sym_pt->type){
//case none:
//case temp:
//case data:
//case pointer:
//case function:
//case subroutine:
//case literal:
//case type_def:
//default:
//}

typedef struct sym {
	name_dx name;
	symbol_t type;     // Symbol type
	
	// Data, Pointer
	bool stat;     // Is it a static data location
	bool constant; // should this data ever be changed again
	bool init;     // Is the data location initialized
	
	// Data and temp
	width_t size;
	
	// literal
	umax value;
	
	// function and subroutine
	bool assembler; // Is this an assembler routine fun or sub
	DS   local;     // Local scope for functions and structures
	// Parameter specification
//	sym_entry * A;
//	sym_entry * B;
//	sym_entry * C;
//	sym_entry * D;
//	sym_entry * E;
//	sym_entry * F;
	// return value
	
	// pointer
	struct sym* dref;
	
	
	// Used by the code generators
	bool live;
}sym_entry;

typedef sym_entry* sym_pt;


/*************************** INTERMEDIATE QUEUE *******************************/

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
}byte_code;

typedef union {
	const sym_entry * symbol; ///< a variable
	umax              value;  ///< a literal
} intermed_arg;


typedef struct icode {
	name_dx           label;    ///< The label, if any, for this operation
	byte_code         op;       ///< The intermediate operator
	name_dx           target;   ///< target of a jump
	bool              arg1_lit; ///< whether arg1 is literal or a symbol
	bool              arg2_lit; ///< whether arg2 is literal or a symbol
	const sym_entry * result;   ///< result of the operation
	intermed_arg      arg1;     ///< first argument
	intermed_arg      arg2;     ///< second argument
} icmd;

/************************ PORTABLE EXECUTABLE FILE ****************************/

typedef struct pexe_header{
	uint64_t magic        ; ///< Magic number
	uint32_t version      ; ///< File version
	uint32_t name_array_sz; ///< Name Array length in bytes
	///< File position for start of symbol table
	///< File position for start of op queue
	///< checksum of everything after the header
} pexe_h;

//typedef struct pexe_symbol{
//	
//} pexe_sym;

//typedef struct pexe_operation{
//	
//} pexe_op;

#endif // _TYPES_H

