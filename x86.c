/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "compiler.h"


/******************************************************************************/
//                               DEFINITIONS
/******************************************************************************/


typedef enum {
	A , B , C  , D  , SI , DI , BP , SP,
	R8, R9, R10, R11, R12, R13, R14, R15
} reg_t;

typedef enum {
	X_MOV,
	X_ADD,
	X_SUB,
	NUM_X86_INST
} x86_inst;

const char * inst_array[NUM_X86_INST] = {
	"mov", "add", "sub"
};

typedef struct blk {
	uint count;
	const icmd * steps[];
} basic_block;


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


/// runs the various functions through their various outputs.
void test_x86(void);


static char * put_num(umax num){
	static char array[20];
	sprintf(array, "0x%llu", num);
	return array;
}

// assumes width is possible in current mode
static char * put_reg(width_t width, reg_t reg, bool B64){
	static char array[4];
	
	if (!B64 && width == byte8)
		crit_error("operand too large for current proccessor mode");
	if (!B64 && reg >SP)
		crit_error("registers R8-R15 are only availible in Long mode");
	
	array[4] = '\0';
	
	switch (width){
	case byte  : array[1] = ' '; array[3] = ' '; break;
	case byte2 : array[1] = ' '; array[3] = 'x'; break;
	case byte3 :
	case byte4 : array[1] = 'e'; array[3] = 'x'; break;
	case byte8 :
	case max_t : array[1] = 'r'; array[3] = 'x'; break;
	case void_t:
	default    : crit_error("something done broke in put_reg()");
	}
	
	switch (reg){
	case A: array[2] = 'a'; break;
	case B: array[2] = 'b'; break;
	case C: array[2] = 'c'; break;
	case D: array[2] = 'd'; break;
	case SI: array[2] = 's'; array[3] = 'i'; break;
	case DI: array[2] = 'd'; array[3] = 'i'; break;
	case BP: array[2] = 'b'; array[3] = 'p'; break;
	case SP: array[2] = 's'; array[3] = 'p'; break;
	case R8: array[2] = '8'; array[3] = ' '; break;
	case R9: array[2] = '9'; array[3] = ' '; break;
	case R10: array[2] = '1'; array[3] = '0'; break;
	case R11: array[2] = '1'; array[3] = '1'; break;
	case R12: array[2] = '1'; array[3] = '2'; break;
	case R13: array[2] = '1'; array[3] = '3'; break;
	case R14: array[2] = '1'; array[3] = '4'; break;
	case R15: array[2] = '1'; array[3] = '5'; break;
	default: crit_error("something done broke in put_reg()");
	}
	
	return array;
}


static void put_operation(icmd * op){
	switch (op -> op){
	case I_NOP:
		break;

	case I_ASS :
		break;

	case I_REF :
		break;

	case I_DREF:
		break;

	case I_NEG :
		break;

	case I_NOT :
		break;

	case I_INV :
		break;

	case I_INC :
		break;

	case I_DEC :
		break;

	case I_MUL:
		break;

	case I_DIV:
		break;

	case I_MOD:
		break;

	case I_EXP:
		break;

	case I_LSH:
		break;

	case I_RSH:
		break;

	case I_ADD :
		break;

	case I_SUB :
		break;

	case I_BAND:
		break;

	case I_BOR :
		break;

	case I_XOR :
		break;

	case I_EQ :
		break;

	case I_NEQ:
		break;

	case I_LT :
		break;

	case I_GT :
		break;

	case I_LTE:
		break;

	case I_GTE:
		break;

	case I_AND:
		break;

	case I_OR :
		break;

	case I_JMP :
		break;

	case I_JZ  :
		break;

	case I_BLK :
		break;

	case I_EBLK:
		break;

	case I_CALL:
		break;

	case I_RTRN:
		break;

	default: crit_error("something done broke in put_operation()");
	}
}


static basic_block * get_blk(void){
	basic_block * blk;
	
	
}

/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void x86 (char * filename, bool B64){
	FILE * outfile;
	icmd * operation;
	
	
	outfile = fopen(filename, "w");
	
	fprintf(outfile,"; a NASM assembler file created by the Omega Compiler\n");
	
	// Set width
	if(B64)
		fprintf(
			outfile,
			"BITS 64 ; tell nasm that we are building for long mode\n"
		);
	else
		fprintf(
			outfile,
			"BITS 32 ; tell nasm that we are building for protected mode\n"
		);
	
	fprintf(outfile,"global\t_start\n");
	
	// executable code
	fprintf(outfile,"\nsection .text\t; Program code\n");
	fprintf(outfile,"_start:\n");
	
	while (operation = DS_dq(global_inst_q)){
		put_operation(operation);
	}
	
	while (operation = DS_dq(sub_inst_q)){
		put_operation(operation);
	}
	
	fprintf(outfile,"\nsection .data\t; Data Section contains constants\n");
	fprintf(outfile,"\nsection .bss\t; Declare static variables\n");
	if (B64) fprintf(outfile,"align 8\n");
	else fprintf(outfile,"align 4\n");
	
	//TODO: static variables
}


