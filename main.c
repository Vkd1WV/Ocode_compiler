/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This program should be build with make. A makefile has been provided.
//
/******************************************************************************/


#include "compiler.h"


int main (int argc, const char** argv){
	char * outfile = NULL;
	bool errors;
	
	/**************************** PARSE COMMAND *******************************/
	
	// occ [-d=debugfile] [-o=outfilename] infile.oc
	for(int i=1; argv[i][0] == '-'; i++){
		switch (argv[i][1]){
			case 'd': debug_fd = fopen(argv[i]+3, "w"); break;
			case 'o': outfile = argv[i]+3;              break;
			default:
				printf("Unrecognized parameter: '%s'.\n", argv[i]);
				exit(EXIT_FAILURE);
		}
	}
	
	yyin = fopen(argv[argc], "r");
	if (yyin == NULL){
		printf("ERROR: file '%s' does not exist.\n", argv[argc]);
		exit(EXIT_FAILURE);
	}
	
	// set the default outfile name
	if (!outfile) {
		outfile=malloc(strlen(argv[argc])+5);
		if (!outfile) exit(EXIT_FAILURE);
		sprintf(outfile, "%s.asm", argv[argc]);
	}
	
	/*************************** INITIALIZATIONS ******************************/
	
	// The build architecture and mode
//	arch=x86;
//	x86_mode=Long;
	
	/***************** PARSING / INTERMEDIATE CODE GENERATION *****************/
	
	Initialize_intermediate(); // Initialize the intermediate code generator
	get_token(); // Initialize the lookahead token
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	fclose(debug_fd);
	
	if(errors){
		puts("Errors were found.");
		exit(EXIT_FAILURE);
	}
	/***************************** OPTIMIZATION *******************************/
	
	/**************************** CODE GENERATION *****************************/
	
	/******************************* Cleanup **********************************/
	
	return EXIT_SUCCESS;
}
