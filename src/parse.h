/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _PARSE_H
#define _PARSE_H

#include "scanner.h"
#include "prog_data.h"

static inline void debug_sym(const char * message, sym_pt sym){
	msg_print(NULL, V_DEBUG, "%s, on line %4d: ", message, Scanner::lnum());
	Print_sym(stderr, sym);
}

static inline void debug_iop(const char * message, iop_pt iop){
	msg_print(NULL, V_DEBUG, "%s, on line %4d: ", message, Scanner::lnum());
	Print_iop(stderr, iop);
}

bool Parse(const char * infile);

#endif // _PARSE_H


