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
void Optomize(Program_data * prog, Instruction_Queue * inst_q);

// parse.cpp
bool Parse(Program_data * d, Scanner * s);

// gen-pexe.c
void pexe (char * filename, const Program_data * prog);

// gen-x86.c
void x86 (char * filename, const Program_data * prog, bool B64);

#endif // _PROTO_H


