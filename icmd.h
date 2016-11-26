/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


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


/******************************************************************************/
//                                C WRAPPERS
/******************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

void emit_iop(
	op_code      op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
);

void Print_icmd(void * iop);

void emit_lbl(name_dx lbl);

#ifdef __cplusplus
}
#endif


