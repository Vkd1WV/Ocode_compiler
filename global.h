/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _GLOBAL_H
#define _GLOBAL_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <data.h>

typedef unsigned long long umax;
typedef unsigned int uint;


/******************************************************************************/
//                                CONSTANTS
/******************************************************************************/


#define ERR_ARR_SZ  100  ///< temp arrays used in error reporting

extern const char * default_out;
extern const char * default_dbg;
extern const char * default_asm;
extern const char * default_pexe;

/******************************************************************************/
//                             GLOBAL VARIABLES
/******************************************************************************/


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


uint verbosity;
EXTERN jmp_buf anewline;  ///< to facilitate error recovery
EXTERN FILE *  debug_fd;  ///< contains a text representation of the instruction queue
char err_array[ERR_ARR_SZ];


#undef EXTERN


/******************************************************************************/
//                                PROTOTYPES
/******************************************************************************/


static inline void crit_error(const char* message){
	fprintf(stderr, "CRITICAL ERROR: %s.\n", message);
	exit(EXIT_FAILURE);
}

#endif // _GLOBAL_H


