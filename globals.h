/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

/**	@file globals.h
 *	This file contains Global variables and constants.
 */

#ifndef _GLOBALS_H
#define _GLOBALS_H

#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/******************************************************************************/
//                            Arbitrary Limits
/******************************************************************************/


/// string length limit for unique compiler generated labels
#define UNQ_NAME_SZ 16
#define ERR_ARR_SZ  100  ///< temp arrays used in error reporting
#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array

// initialized in globals.c
extern const char * default_out;
extern const char * default_dbg;
extern const char * default_asm;
extern const char * default_pexe;
extern const char * byte_code_dex[NUM_I_CODES];

/******************************* DEBUGGING ************************************/

uint verbosity;
EXTERN jmp_buf anewline;  ///< to facilitate error recovery
EXTERN FILE *  debug_fd;  ///< contains a text representation of the instruction queue

/*************************** SCANNER TO PARSER ********************************/

EXTERN token_t token;     ///< global lookahead token
EXTERN uint    block_lvl; ///< number of leading tabs on the current line
EXTERN umax    yynumber;  ///< numbers passed to the parser by T_NUM

// Global variables provided by the scanner
#ifndef _SCANNER_L
extern int     yylineno;
extern char *  yytext;
extern FILE *  yyin;
#endif

/*********************** INTERMEDIATE REPRESENTATION **************************/

EXTERN DS      symbols;       ///< symbol table
EXTERN DS      global_inst_q; ///< a global instruction queue
EXTERN DS      sub_inst_q;    ///< an instruction queue for subroutines
EXTERN char *  name_array;    ///< dynamic array for symbol and label names

/***************************** CODE GENERATOR *********************************/

//EXTERN FILE*   out_fd;
//EXTERN mode_t  x86_mode; ///< the proccessor mode we are compiling for
//EXTERN arch_t  arch;     ///< the target architecture

// defined in globals.c
//extern const char* _e_noimp;

#endif // _GLOBALS_H
