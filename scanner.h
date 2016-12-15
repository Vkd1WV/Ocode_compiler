/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _SCANNER_H
#define _SCANNER_H


/*	This file is just an interface between parser.c and scanner.c and doesn't
 *	need to be global.
 */

/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


typedef enum {
	T_EOF , // yyterminate returns a 0 automatically
	T_NL  , // sets block_lvl
	T_NUM , // sets yynumber
	T_STR , // sets yytext
	T_CHAR, // sets yynumber
	/*	char will eventually be a variable width encoding, but for now it is
	 *	just byte
	 */

	/****************************** Operators *********************************/

	// order of operations
	T_OPAR ,
	T_CPAR ,
	T_OBRC ,
	T_CBRC ,
	T_OBRK ,
	T_CBRK ,

	// unary
	T_DEC  ,
	T_INC  ,
	T_REF  ,
	T_DREF ,
	T_NOT  ,
	T_INV  ,

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

	T_LIST ,

	/************************** Control statements ****************************/

	T_LBL  ,
	T_JMP  ,
	T_IF   ,
	T_ELSE ,
	T_SWTCH,
	T_CASE ,
	T_DFLT ,
	T_WHILE,
	T_DO   ,
	T_BRK  ,
	T_CNTN ,
	T_TRY  ,
	T_THRW ,
	T_CTCH ,
	T_FOR  ,
	T_RTRN ,
	
	/**************************** Declared Names ******************************/

	T_NAME,      // sets yytext
	T_N_SUB,  // Named subroutine
	T_N_FUN,  // Named function or storage
	T_N_TYPE, // Named Type

	/***************************** Declarations *******************************/

	T_OPR , // Operator Declaration
	T_SUB , // Subroutine
	T_FUN , // Function
	T_TYPE, // Type Declaration

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
	
	// Parameter Modes
	T_IN,
	T_OUT,
	T_BI,
	NUM_TOKENS

} token_t;

typedef struct{
	sym_pt scope;
	DS     inst_q;
} prg_blk;


/******************************************************************************/
//                            GLOBAL VARIABLES
/******************************************************************************/


#ifdef _SCANNER_L
	#define EXTERN
#else
	#define EXTERN extern
	
	// Global variables provided by the scanner
	extern int     yylineno;
	extern char *  yytext;
	extern FILE *  yyin;
#endif

EXTERN DS      scope_stack; ///< keep track of the current scope and inst queue
EXTERN DS      symbols;     ///< symbol table
EXTERN sym_pt  yysymbol;    ///< symbols passed to the parser by T_NAME_XXX
EXTERN token_t token;       ///< global lookahead token
EXTERN uint    block_lvl;   ///< number of leading tabs on the current line
EXTERN umax    yynumber;    ///< numbers passed to the parser by T_NUM

EXTERN const char * token_dex[NUM_TOKENS]
#ifdef _SCANNER_L
= {
	// Primary
	"T_EOF", "T_NL", "T_NUM", "T_STR", "T_CHAR",
	/****************************** Operators *********************************/
	"(",")","{","}","[","]",
	// Unary
	"T_DEC  "," T_INC  "," T_REF  ","@"," T_NOT  "," T_INV  ",
	// Additive
	"T_PLUS "," T_MINUS"," T_BAND ", "T_BOR", "T_BXOR",
	// Multiplicative
	"T_MUL  "," T_MOD  "," T_DIV  "," T_EXP  "," T_LSHFT"," T_RSHFT",
	// Comparative
	"="," T_NEQ","<",">"," T_LTE"," T_GTE",
	// boolean
	"T_AND"," T_OR ",
	// Assignment
	":="  , "T_LSH_A", "T_RSH_A", "T_ADD_A", "T_SUB_A", "T_MUL_A", "T_DIV_A",
	"T_MOD_A", "T_AND_A", "T_OR_A ", "T_XOR_A",
	
	"T_LIST ",
	/************************** Control statements ****************************/
	"T_LBL"  , "T_JMP" , "T_IF" , "T_ELSE", "T_SWTCH", "T_CASE", "T_DFLT",
	"T_WHILE", "T_DO"  , "T_BRK", "T_CNTN", "T_TRY"  , "T_THRW", "T_CTCH",
	"T_FOR"  , "T_RTRN",
	/**************************** Declared Names ******************************/
	"T_NAME", "T_NAME_SUB", "T_NAME_FUN", "T_NAME_TYPE",
	/***************************** Declarations *******************************/
	"T_OPR "," T_SUB "," T_FUN "," T_TYPE",
	// Word declarations
	"T_8   "," T_16  "," T_32  "," T_64  "," T_WORD"," T_MAX ",
	// pointer declaration
	"T_PTR",
	// Qualifers
	"T_CONST", "T_STATIC", "T_ASM",
	// Parameter Modes
	"T_IN", "T_OUT", "T_BI"
}
#endif // _SCANNER_L
;

#undef EXTERN


/******************************************************************************/
//                              FROM SCANNER.L
/******************************************************************************/


token_t yylex(void);

static inline void get_token(void){
	token=yylex();
}


/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/


static inline void parse_error(const char * message){
	fprintf(stderr, "CODE ERROR: %s, on line %d.\n", message, yylineno);
	if(token != T_EOF && token != T_NL)
		while( (token = yylex()) != T_NL && token != T_EOF );
	fprintf(stderr, "continuing...\n");
	longjmp(anewline, 1);
}


#endif // _SCANNER_H


