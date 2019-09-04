/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

/** This file contains the interface between the flex generated component and
the rest of the scanner.
*/

#ifndef _FLEX_H
#define _FLEX_H

/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include "token.h"
#include <util/types.h>
#include "errors.h"


/******************************************************************************/
//                          SCANNER MODULE VARIABLES
/******************************************************************************/


#ifdef _GLOBAL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/// number of leading tabs on the current line
EXTERN uint tabs_this_ln;

// Global variables provided by the scanner
#ifndef _LEX_L
	extern int    yylineno;
	extern char * yytext;
	extern int    yyleng; ///< the length of yytext
	extern FILE * yyin;
#endif

#undef EXTERN

/******************************************************************************/
//                                PROTOTYPES
/******************************************************************************/


/// Report errors in the scanner
static inline void scan_error(const char * message){
	msg_print(logfile, V_ERROR, "SCANNER: %s, on line %d\n", 
		message, yylineno
	);
}

#define YY_DECL token_t yylex(void)
token_t yylex(void);


#endif // _FLEX_H


