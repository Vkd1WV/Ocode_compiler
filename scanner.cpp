/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include <stdio.h>
#include "errors.h"
#include "scanner.h" // interface for the flex component
#include "lex.h"

/******************************************************************************/
//                            SCANNER FUNCTIONS
/******************************************************************************/

Scanner::Scanner(char * filename){
	
	// set the infile
	if(filename){
		sprintf(err_array, "Reading from: %s", filename);
		info_msg(err_array);
		yyin = fopen(filename, "r");
		if(!yyin) {
			sprintf(err_array, "No such file: %s", filename);
			crit_error(err_array);
		}
	}
	else{
		info_msg("Reading from: stdin");
		yyin = stdin;
	}
	
	next_token(); // initialize the token
}


void Scanner::next_token(void){
	static unsigned int tabs_last_ln;
	
	if(tabs_last_ln == tabs_this_ln) {
		scan_token = yylex();
		return;
	}
}


