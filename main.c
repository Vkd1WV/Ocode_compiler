/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This program should be build with make. A makefile has been provided.
//
/******************************************************************************/


#include "compiler.h"


int main (int argc, const char** argv){
	char * outfile = NULL, * infile = NULL;
	char default_outfile[8] = "out.asm";
	bool errors;
	
/*************************** PARSE COMMAND-LINE *******************************/
	
	// occ [-d=debugfile] [-o=outfilename] [infile.oc]
	for(int i=1; i<argc; i++){
		if(argv[i][0] == '-'){
			switch (argv[i][1]){
				case 'd': debug_fd = fopen(argv[i]+3, "w"); break;
				case 'o': outfile = argv[i]+3;              break;
				default:
					printf("Unrecognized parameter: '%s'.\n", argv[i]);
					exit(EXIT_FAILURE);
			}
		}
		else if (i == argc-1) {
			infile = argv[argc-1];
			yyin = fopen(infile, "r");
			// yyin defaults to stdin
			if (yyin == NULL){
				printf("ERROR: file '%s' does not exist.\n", argv[argc]);
				exit(EXIT_FAILURE);
			}
		} 
		else {
			printf("ERROR: Unrecognized command-line option '%s'.\n", argv[i]);
			exit(EXIT_FAILURE);
		}
	}
	
	// set the default outfile name
	if (!outfile) {
		if (infile){
			outfile=malloc(strlen(infile)+5);
			if (!outfile) crit_error("Out of memory");
			sprintf(outfile, "%s.asm", infile);
		}
		else{ // use the default outfile
			outfile = default_outfile;
		}
	}
	
	/*************************** INITIALIZATIONS ******************************/
	
	// The build architecture and mode
	arch=x86;
	x86_mode=Long;
	
	/***************** PARSING / INTERMEDIATE CODE GENERATION *****************/
	
	Initialize_intermediate(); // Initialize the intermediate code generator
	get_token(); // Initialize the lookahead token
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	if (debug_fd){
		Dump_symbols();
		fclose(debug_fd);
	}
	
	if(errors){
		puts("Errors were found.");
		exit(EXIT_FAILURE);
	}
	/***************************** OPTIMIZATION *******************************/
	
	/**************************** CODE GENERATION *****************************/
	
	/******************************* Cleanup **********************************/
	
	return EXIT_SUCCESS;
}
