/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _PARSER_H
#define _PARSER_H


/*	Most of this file is just an interface with scanner.c and doesn't need to be
 *	global. The Parse() prototype can go to global.c and this can be renamed
 *	scanner.h and only included in parse.c
 */

/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


typedef enum {
	T_EOF , // yyterminate returns a 0 automatically
	T_NL  , // sets block_lvl
	T_NUM , // sets yynumber
	T_NAME, // sets yytext
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
	T_END  , // End of Subroutine or function definition


	/***************************** Declarations *******************************/

	T_OPR , // Operator Declaration

	// Word declarations
	T_8   ,
	T_16  ,
	T_24  ,
	T_32  ,
	T_64  ,
	T_WORD,
	T_MAX ,

	// pointer declaration
	T_PTR,
	T_TO ,

	T_SUB , // Subroutine
	T_FUN , // Function
	T_TYPE, // Type Declaration

	// Qualifers
	T_CONST ,
	T_STATIC,
	T_ASM     // Assembler Routine

} token_t;

//typedef uint16_t token_t;


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

EXTERN token_t token;     ///< global lookahead token
EXTERN uint    block_lvl; ///< number of leading tabs on the current line
EXTERN umax    yynumber;  ///< numbers passed to the parser by T_NUM

#undef EXTERN


/******************************************************************************/
//                              FROM SCANNER.L
/******************************************************************************/


token_t yylex(void);


/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/


static inline void parse_error(const char * message){
	fprintf(stderr, "CODE ERROR: %s, on line %d.\n", message, yylineno);
	while( (token = yylex()) != T_NL );
	fprintf(stderr, "continuing...\n");
	longjmp(anewline, 1);
}


/******************************************************************************/
//                             GLOBAL PROTOTYPES
/******************************************************************************/





#endif // _PARSER_H


