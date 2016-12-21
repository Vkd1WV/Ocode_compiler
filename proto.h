/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _PROTO_H
#define _PROTO_H


#include "my_types.h"
#include "prog_data.h"
#include "scanner.h"

// opt.c
void Optomize(Instruction_Queue * inst_q);

// parse.cpp
bool Parse(const char * infile);

// gen-pexe.c
void pexe (char * filename);

// gen-x86.c
void x86 (char * filename, bool B64);

#endif // _PROTO_H


