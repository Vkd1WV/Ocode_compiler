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
	
	//debug_msg("Scanner(): start");
	
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
	
	//debug_msg("Scanner(): stop");
}


void Scanner::next_token(void){
	static unsigned int tabs_last_ln;
	
	//debug_msg("Scanner::next_token(): start");
	
	if(tabs_last_ln == tabs_this_ln) {
		//debug_msg("Equal");
		
		scan_token = yylex();
		scan_text  = yytext;
		if(scan_token != T_NL) line_num = yylineno;
	}
	else if(tabs_last_ln > tabs_this_ln){
		//debug_msg(">");
		
		scan_token = T_OUTD;
		scan_text = NULL;
		
		if(tabs_last_ln > 0) tabs_last_ln--;
		else scan_error("Scanner::next_token(): ");
	}
	else if(tabs_last_ln < tabs_this_ln){
		//debug_msg("<");
		
		scan_token = T_IND;
		scan_text = NULL;
		tabs_last_ln++;
	}
	
	//debug_msg("Scanner::next_token(): stop");
	
}


