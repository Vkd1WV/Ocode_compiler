/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _TOKEN_H
#define _TOKEN_H

/** contains the definition for token_t: the scanner tokens.
 */


typedef enum {
	T_EOF , // yyterminate returns a 0 automatically
	T_SERR, // a scanner error
	
	// Formatting
	T_NL  ,
	T_TABS,
	T_IND,
	T_OUTD,
	
	// Primary types
	T_NUM ,
	T_STR ,
	T_CHAR,
	/*	char will eventually be a variable width encoding, but for now it is
	 *	just byte
	 */

	/****************************** Operators *********************************/

	// order of operations
	T_OPAR ,
	T_CPAR ,

	// unary
	T_DEC  ,
	T_INC  ,
	T_REF  ,
	T_DREF ,
	T_NOT  ,
	T_INV  ,

	// postfix
	T_PEROD,
	T_OBRK ,
	T_CBRK ,

	// basic
	T_PLUS ,
	T_MINUS,
	T_BAND ,
	T_BOR  ,
	T_BXOR ,

	// multiplicative
	T_MUL  ,
	T_MOD  ,
	T_DIV  ,
	T_EXP  ,
	T_LSHFT,
	T_RSHFT,

	// comparative
	T_EQ   ,
	T_NEQ  ,
	T_LT   ,
	T_GT   ,
	T_LTE  ,
	T_GTE  ,

	// boolean
	T_AND  ,
	T_OR   ,

	// assignment
	T_ASS  , // assignments in function calls are pipes
	T_LSH_A,
	T_RSH_A,
	T_ADD_A,
	T_SUB_A,
	T_MUL_A,
	T_DIV_A,
	T_MOD_A,
	T_AND_A,
	T_OR_A ,
	T_XOR_A,
	
	T_OBRC ,
	T_CBRC ,

	T_COMA,
	
	/************************** Control statements ****************************/
	
	T_LBL  ,
	T_JMP  ,
	T_BRK  ,
	T_CNTN ,
	T_RTRN ,
	
	T_IF   ,
	T_ELSE ,
	T_SWTCH,
	T_CASE ,
	T_DFLT ,
	T_WHILE,
	T_DO   ,
	T_FOR  ,
	
	T_TRY  ,
	T_THRW ,
	T_CTCH ,
	
	/**************************** Declared Names ******************************/

	T_NAME,   // sets yytext
	T_N_STRG, // Named Storage
	T_N_FUN,  // Named function or storage
	T_N_SUB,  // Named subroutine
	T_N_TYPE, // Named Type
	T_N_CLSS, // Named Class

	/***************************** Declarations *******************************/

	T_OPR , // Operator Declaration
	T_SUB , // Subroutine
	T_FUN , // Function
	T_TYPE, // Type Declaration
	T_VOID, // void declaration

	// Word declarations
	T_8   ,
	T_16  ,
	T_32  ,
	T_64  ,
	T_WORD,
	T_MAX ,

	// pointer declaration
	T_PTR,

	// Qualifers
	T_CONST ,
	T_STATIC,
	T_ASM,     // Assembler Routine
	T_ASM_BLK,
	
	// Parameter Modes
	T_IN,
	T_OUT,
	T_BI,
	NUM_TOKENS

} token_t;


#ifdef _GLOBAL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

// token_dex[] contains user readable values for each token.

EXTERN const char * token_dex[NUM_TOKENS]
#ifdef _GLOBAL_C
	= {
		// Formatting
		"T_EOF", "T_SERR", "T_NL", "T_TABS", "T_IND", "T_OUTD",
		// Primary
		"T_NUM", "T_STR", "T_CHAR",
		/**************************** Operators *******************************/
		"T_OPAR","T_CPAR",
		// Unary
		"T_DEC","T_INC","T_REF","T_DREF", "T_NOT", "T_INV",
		// Postfix
		"T_PEROD", "T_OBRK","T_CBRK",
		// Additive
		"T_PLUS", "T_MINUS", "T_BAND", "T_BOR", "T_BXOR",
		// Multiplicative
		"T_MUL", "T_MOD", "T_DIV", "T_EXP", "T_LSHFT", "T_RSHFT",
		// Comparative
		"T_EQ", "T_NEQ", "T_LT", "T_GT", "T_LTE", "T_GTE",
		// boolean
		"T_AND", "T_OR",
		// Assignment
		"T_ASS", "T_LSH_A", "T_RSH_A", "T_ADD_A", "T_SUB_A", "T_MUL_A",
		"T_DIV_A", "T_MOD_A", "T_AND_A", "T_OR_A ", "T_XOR_A",

		"T_OBRC","T_CBRC", "T_COMA",
		/************************ Control statements **************************/
		"T_LBL", "T_JMP", "T_BRK", "T_CNTN", "T_RTRN",
		"T_IF", "T_ELSE", "T_SWTCH", "T_CASE", "T_DFLT",
		"T_WHILE", "T_DO", "T_FOR",
		"T_TRY", "T_THRW", "T_CTCH",
		/************************** Declared Names ****************************/
		"T_NAME", "T_N_STRG", "T_N_FUN", "T_N_SUB", "T_N_TYPE", "T_N_CLSS",
		/*************************** Declarations *****************************/
		"T_OPR", "T_SUB", "T_FUN", "T_TYPE", "T_VOID",
		// Word declarations
		"T_8", "T_16", "T_32", "T_64", "T_WORD", "T_MAX",
		// pointer declaration
		"T_PTR",
		// Qualifers
		"T_CONST", "T_STATIC", "T_ASM",
		// Parameter Modes
		"T_IN", "T_OUT", "T_BI"
	}
#endif // _GLOBALS_C
;


#undef EXTERN


#endif // _TOKEN_H


