/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This program should be build with make. A makefile has been provided.
//
/******************************************************************************/


/******************************************************************************/
//                                 INCLUDES
/******************************************************************************/


#include "parse.h"


/******************************************************************************/
//                        GLOBAL CONSTANTS & VARIABLES
/******************************************************************************/


token_t token; // global lookahead token
FILE* outfile;
mode_t x86_mode;


/******************************************************************************/
//                             BASIC FUNCTIONS
/******************************************************************************/


static inline void init(void){
	get_token();
}

// create and return a pointer to a unique label
char* new_label(void){
	static int i;
	static char label[UNQ_LABEL_SZ];
	
	sprintf(label, "__%04d", i++); // use __ to prevent collisions
	return label;
}

void Move(reg_t dest, regsz_t dsize, reg_t src, regsz_t ssize){
	char output[22] = "mov     x,   x";
	
	if(x86_mode<Long && (dsize>dword || ssize>dword))
		error("operand too large for current proccessor mode");
	if(x86_mode<Long && ((dest>R7 && dest<=R15) || (src>R7 && src<=R15)))
		error("registers R8-R15 are only availible in Long mode");
	
	
	if(dsize > ssize)
		// extend
		{}
	else if (dsize < ssize)
		// better be a recast
		{}
	
	switch (dsize){
	case byte:
		output[8] = 'l';
		break;
	case dword:
		output[6] = 'e';
		break;
	case qword:
		output[6] = 'r';
		break;
	default:
		error("something done broke in Move()");
	}
	
	switch (ssize){
	case byte:
		output[13] = 'l';
		break;
	case dword:
		output[11] = 'e';
		break;
	case qword:
		output[11] = 'r';
		break;
	default:
		error("something done broke in Move()");
	}
	
	switch (src){
	case R0:
	case A:
		output[12]='a';
		break;
	case R1:
	case B:
		output[12]='b';
		break;
	case R2:
	case C:
		output[12]='c';
		break;
	case R3:
	case D:
		output[12]='d';
		break;
	case R4:
	case SI:
		output[12]='s';
		output[13]='i';
		break;
	case R5:
	case DI:
		output[12]='d';
		output[13]='i';
		break;
	case R6:
	case BP:
		output[12]='b';
		output[13]='p';
		break;
	case R7:
	case SP:
		output[12]='s';
		output[13]='p';
		break;
	}
	
	switch (dest){
	case R0:
	case A:
		output[7]='a';
		break;
	case R1:
	case B:
		output[7]='b';
		break;
	case R2:
	case C:
		output[7]='c';
		break;
	case R3:
	case D:
		output[7]='d';
		break;
	case R4:
	case SI:
		output[7]='s';
		output[8]='i';
		break;
	case R5:
	case DI:
		output[7]='d';
		output[8]='i';
		break;
	case R6:
	case BP:
		output[7]='b';
		output[8]='p';
		break;
	case R7:
	case SP:
		output[7]='s';
		output[8]='p';
		emit_cmnt("Probably shouldn't be writing to SP");
		break;
	}
	emit_cmd(output);
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


int main (void){
	outfile=stdout;
	x86_mode=Long;
	init();
	
	fprintf(outfile,"; a NASM object file created by the Omega Compiler\n");
	fprintf(
		outfile,
		"BITS 64 ; tell nasm that we are building for 64-bit\n"
	);
	fprintf(outfile,"global\t_start\n");
	
	// executable code
	fprintf(outfile,"\nsection .text\t; Program code\n");
	fprintf(outfile,"_start:\n");
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	// constants
	fprintf(outfile,"\nsection .data\t; Data Section contains constants\n");
	fprintf(outfile,"\nsection .bss\t; Declare static variables\n");
	
	return EXIT_SUCCESS;
}
