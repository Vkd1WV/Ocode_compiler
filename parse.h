/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _PARSER_H
#define _PARSER_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


typedef uint16_t token_t;


/******************************************************************************/
//                            GLOBAL VARIABLES
/******************************************************************************/


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN token_t token;     ///< global lookahead token
EXTERN uint    block_lvl; ///< number of leading tabs on the current line
EXTERN umax    yynumber;  ///< numbers passed to the parser by T_NUM

// Global variables provided by the scanner
#ifndef _SCANNER_L
extern int     yylineno;
extern char *  yytext;
extern FILE *  yyin;
#endif

#undef EXTERN


/******************************************************************************/
//                              FROM SCANNER.L
/******************************************************************************/


token_t yylex(void);
char * get_name(void);


/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/


static inline void get_token(void){
	token=yylex();
}

static inline void parse_error(const char * message){
	fprintf(stderr, "CODE ERROR: %s, on line %d.\n", message, yylineno);
	while( (token = yylex()) != T_NL );
	fprintf(stderr, "continuing...\n");
	longjmp(anewline, 1);
}

static inline void expected(const char* thing){
	char temp_array[ERR_ARR_SZ];
	sprintf(temp_array, "Expected %s, found %s", thing, yytext);
	parse_error(temp_array);
}


/******************************************************************************/
//                             GLOBAL PROTOTYPES
/******************************************************************************/


void Parse(yuck_t * arg_pt);


#endif // _PARSER_H


