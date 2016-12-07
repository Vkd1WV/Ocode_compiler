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
//                           DEPENDENCY HEADERS
/******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <data.h>


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


typedef unsigned long long umax;
typedef unsigned int uint;

typedef enum {
	V_QUIET,
	V_ERROR,
	V_WARN,
	V_INFO,
	V_DEBUG
} verb_t;


/******************************************************************************/
//                                CONSTANTS
/******************************************************************************/


#define DEFAULT_VERBOSITY (verb_t)V_WARN
#define ERR_ARR_SZ  100  ///< temp arrays used in error reporting


/******************************************************************************/
//                             GLOBAL VARIABLES
/******************************************************************************/


#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


EXTERN verb_t  verbosity; ///< set in Set_files() and never changed again
EXTERN jmp_buf anewline;  ///< to facilitate error recovery
EXTERN char    err_array[ERR_ARR_SZ]; ///< Temporary string for messages


#undef EXTERN


/******************************************************************************/
//                         GLOBAL INLINE FUNCTIONS
/******************************************************************************/


static inline void crit_error(const char* message){
	if (verbosity >= V_ERROR)
		fprintf(stderr, "CRITICAL ERROR: %s.\n", message);
	exit(EXIT_FAILURE);
}

static inline void err_msg(const char * message){
	if (verbosity >= V_ERROR)
		fprintf(stderr, "ERROR: %s.\n", message);
}

static inline void warn_msg(const char * message){
	if (verbosity >= V_WARN)
		fprintf(stderr, "ERROR: %s.\n", message);
}

static inline void info_msg(const char * message){
	if (verbosity >= V_INFO)
		fprintf(stderr, "INFO: %s.\n", message);
}

static inline void debug_msg(const char * message){
	if (verbosity >= V_DEBUG)
		fprintf(stderr, "DEBUG: %s.\n", message);
}


/******************************************************************************/
//                            COMPILER HEADERS
/******************************************************************************/


#include "prog_data.h"


/******************************************************************************/
//                        GLOBAL FUNCTION PROTOTYPES
/******************************************************************************/

void Parse(Program_data data, FILE * arg_pt);

void Optomize(Program_data prog);

void x86  (char * filename, const Program_data prog, bool B64);
void pexe (char * filename, const Program_data prog          );


#endif // _GLOBAL_H


