/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "compiler.h"
#include "cmd_line.h"


int main (int argc, char** argv){
	yuck_t argp[1];
	char * outfile = NULL;
	char ** filenames;
	bool errors;
	
/*************************** PARSE COMMAND-LINE *******************************/
	
	// Using yuck to parse
	yuck_parse(argp, argc, argv);
	if(argp->dashv_flag) verbose=true;
	if(verbose) print_yuck_results(argp);
	
/******************************** SET FILES ***********************************/
	
	if (verbose) puts("\nSETINGS");
	
	// Set the debug file
	if(argp->debug_arg) {
		if (verbose) printf("Setting %s as a debug file.\n", argp->debug_arg);
		debug_fd = fopen(argp->debug_arg, "w");
	}
	
	// Set the infile
	if(argp->nargs>1) crit_error("Too many arguments");
	if(argp->nargs){
		if (verbose) printf("Setting %s as the input file.\n", *argp->args);
		yyin = fopen(*argp->args, "r");
	}
	else if (verbose) puts("Setting stdin as the input file.");
	
	// Set the outfile
	if(argp->outfile_arg) outfile = argp->outfile_arg;
	else {
		if (argp->nargs){
			outfile=malloc(strlen(*argp->args)+5);
			if (!outfile) crit_error("Out of memory");
			sprintf(outfile, "%s.asm", *argp->args);
		}
		else // use the default outfile
			outfile = default_outfile;
	}
	if (verbose)
			printf("Setting %s as the output file.\n", outfile);
	
	puts("");
	
	/*************************** INITIALIZATIONS ******************************/
	
	
	/***************** PARSING / INTERMEDIATE CODE GENERATION *****************/
	
	/* I would like to be able to proccess multiple files
	   Each file will create its own .pexe file in /tmp which can then be
	   proccessed in turn.
	   I need a temporary file facility
	 */
	
	// print the file we are currently parsing
	if(argp->nargs) printf("%s\n", *argp->args);
	
	Initialize_intermediate(); // Initialize the intermediate code generator
	get_token(); // Initialize the lookahead token
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	if (debug_fd){
		fputs("\n\n", debug_fd);
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
