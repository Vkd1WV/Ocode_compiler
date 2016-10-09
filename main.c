/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This program should be build with make. A makefile has been provided.
//
/******************************************************************************/


#include "compiler.h"

/*void Move(reg_t dest, regsz_t dsize, reg_t src, regsz_t ssize){*/
/*	char output[22] = "mov     x,   x";*/
/*	*/
/*	if(x86_mode<Long && (dsize>dword || ssize>dword))*/
/*		error("operand too large for current proccessor mode");*/
/*	if(x86_mode<Long && ((dest>R7 && dest<=R15) || (src>R7 && src<=R15)))*/
/*		error("registers R8-R15 are only availible in Long mode");*/
/*	*/
/*	*/
/*	if(dsize > ssize)*/
/*		// extend*/
/*		{}*/
/*	else if (dsize < ssize)*/
/*		// better be a recast*/
/*		{}*/
/*	*/
/*	switch (dsize){*/
/*	case byte:*/
/*		output[8] = 'l';*/
/*		break;*/
/*	case dword:*/
/*		output[6] = 'e';*/
/*		break;*/
/*	case qword:*/
/*		output[6] = 'r';*/
/*		break;*/
/*	default:*/
/*		error("something done broke in Move()");*/
/*	}*/
/*	*/
/*	switch (ssize){*/
/*	case byte:*/
/*		output[13] = 'l';*/
/*		break;*/
/*	case dword:*/
/*		output[11] = 'e';*/
/*		break;*/
/*	case qword:*/
/*		output[11] = 'r';*/
/*		break;*/
/*	default:*/
/*		error("something done broke in Move()");*/
/*	}*/
/*	*/
/*	switch (src){*/
/*	case R0:*/
/*	case A:*/
/*		output[12]='a';*/
/*		break;*/
/*	case R1:*/
/*	case B:*/
/*		output[12]='b';*/
/*		break;*/
/*	case R2:*/
/*	case C:*/
/*		output[12]='c';*/
/*		break;*/
/*	case R3:*/
/*	case D:*/
/*		output[12]='d';*/
/*		break;*/
/*	case R4:*/
/*	case SI:*/
/*		output[12]='s';*/
/*		output[13]='i';*/
/*		break;*/
/*	case R5:*/
/*	case DI:*/
/*		output[12]='d';*/
/*		output[13]='i';*/
/*		break;*/
/*	case R6:*/
/*	case BP:*/
/*		output[12]='b';*/
/*		output[13]='p';*/
/*		break;*/
/*	case R7:*/
/*	case SP:*/
/*		output[12]='s';*/
/*		output[13]='p';*/
/*		break;*/
/*	}*/
/*	*/
/*	switch (dest){*/
/*	case R0:*/
/*	case A:*/
/*		output[7]='a';*/
/*		break;*/
/*	case R1:*/
/*	case B:*/
/*		output[7]='b';*/
/*		break;*/
/*	case R2:*/
/*	case C:*/
/*		output[7]='c';*/
/*		break;*/
/*	case R3:*/
/*	case D:*/
/*		output[7]='d';*/
/*		break;*/
/*	case R4:*/
/*	case SI:*/
/*		output[7]='s';*/
/*		output[8]='i';*/
/*		break;*/
/*	case R5:*/
/*	case DI:*/
/*		output[7]='d';*/
/*		output[8]='i';*/
/*		break;*/
/*	case R6:*/
/*	case BP:*/
/*		output[7]='b';*/
/*		output[8]='p';*/
/*		break;*/
/*	case R7:*/
/*	case SP:*/
/*		output[7]='s';*/
/*		output[8]='p';*/
/*		emit_cmnt("Probably shouldn't be writing to SP");*/
/*		break;*/
/*	}*/
/*	emit_cmd(output);*/
/*}*/



int main (int argc, const char** argv){
	sym_entry* sym_pt; // the symbol table
	
	
	/*************************** INITIALIZATIONS ******************************/
	
	
	// The output file
	if (argc > 1) outfile=fopen(argv[1], "w");
	else outfile=stdout;
	
	// The build architecture and mode
/*	arch=x86;*/
/*	x86_mode=Long;*/
	
	// Initialize the lookahead token
	get_token();
	
	// Initialize the symbol table
	global_symbols=new_DS('l');
	
	// Block_lvl is initialized in scanner.l
	
	// boilerplate in the output
/*	fprintf(outfile,"; a NASM object file created by the Omega Compiler\n");*/
/*	fprintf(*/
/*		outfile,*/
/*		"BITS 64 ; tell nasm that we are building for 64-bit\n"*/
/*	);*/
/*	fprintf(outfile,"global\t_start\n");*/
	
	
	fprintf(outfile,"#Omnicode Intermidiate File\n");
	
	
	/***************************** COMPILATION ********************************/
	
	
	// executable code
/*	fprintf(outfile,"\nsection .text\t; Program code\n");*/
/*	fprintf(outfile,"_start:\n");*/
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	
	/**************************** DATA SECTIONS *******************************/
	
	
	//fprintf(outfile,"\nsection .data\t; Data Section contains constants\n");
/*	fprintf(outfile,"\nsection .bss\t; Declare static variables\n");*/
/*	fprintf(outfile,"align 8\n");*/
	
	// Dump the symbol Table
	
	fprintf(outfile,"\n#Symbol Table\n");
	
	pview(global_symbols, 0);
	while((sym_pt=view_next(global_symbols))){
		fprintf(outfile, "%p: %s\n", (void*)sym_pt, sym_pt->name);
	}
	
	
	/******************************* Cleanup **********************************/
	
	
	fclose(outfile);
	return EXIT_SUCCESS;
}
