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

typedef enum {
	V_QUIET,
	V_ERROR,
	V_WARN,
	V_NOTE,
	V_INFO,
	V_DEBUG
} verb_t;


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


verb_t verbosity;

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

static inline void notice_msg(const char * message){
	if (verbosity >= V_NOTE)
		fprintf(stderr, "NOTICE: %s.\n", message);
}

static inline void info_msg(const char * message){
	if (verbosity >= V_INFO)
		fprintf(stderr, "INFO: %s.\n", message);
}

static inline void debug_msg(const char * message){
	if (verbosity >= V_DEBUG)
		fprintf(stderr, "DEBUG: %s.\n", message);
}


#endif // _GLOBAL_H


