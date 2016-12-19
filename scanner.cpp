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
#include "scope.h"


/******************************************************************************/
//                              STATIC DATA
/******************************************************************************/


sym_pt  Scanner::scan_sym;
char *  Scanner::scan_text;
token_t Scanner::scan_token;
uint    Scanner::txtlen;
uint    Scanner::line_num;


/******************************************************************************/
//                           MEMBER FUNCTIONS
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
Scanner::~Scanner(void){
	
	debug_msg("Scanner::~Scanner(): start");
	fclose(yyin);
	debug_msg("Scanner::~Scanner(): stop");
}


void Scanner::next_token(void){
	static uint    tabs_last_ln;
	static token_t token_buf;
	static bool    denting;
	
	//debug_msg("Scanner::next_token(): start");
	
	// We can only call yylex() if we are not currently denting
	if(!denting) scan_token = yylex();
	
	// If we're not denting
	if(!denting && scan_token != T_NL && tabs_last_ln != tabs_this_ln){
		denting = true;
		token_buf = scan_token;
	}
	else {
		scan_text = yytext;
		txtlen    = yyleng;
	}
	
	// If we're denting
	if(denting && tabs_last_ln > tabs_this_ln){
		sprintf(err_array, "%u > %u", tabs_last_ln, tabs_this_ln);
		debug_msg(err_array);
		scan_token = T_OUTD;
		scan_text = NULL;
		txtlen    = 0;
		
		if(tabs_last_ln > 0) tabs_last_ln--;
		else scan_error("Scanner::next_token(): ");
	}
	else if(denting && tabs_last_ln < tabs_this_ln){
		sprintf(err_array, "%u < %u", tabs_last_ln, tabs_this_ln);
		debug_msg(err_array);
		scan_token = T_IND;
		scan_text = NULL;
		txtlen    = 0;
		
		tabs_last_ln++;
	}
	else if (denting){ // we're denting but tabs are now equal
		denting = false;
		scan_token = token_buf;
		scan_text  = yytext;
		txtlen     = yyleng;
	}
	
	// set the line number
	if(scan_token != T_NL) line_num = yylineno;
	
	
	if(scan_token == T_STR || scan_token == T_CHAR || scan_token == T_NUM)
		scan_sym = add_lit_sym(scan_token, scan_text);
	else if(scan_token == T_NAME)
		scan_sym = Scope_Stack::bind(scan_token, scan_text);
	else scan_sym = NULL;
	
	//debug_msg("Scanner::next_token(): stop");
	
}


sym_pt Scanner::add_lit_sym(token_t token, const char * str){
	sym_pt sym=NULL;
	
	if(token == T_NUM){ // process the number
		sym = Program_data::new_var(st_lit_int);
		sym->set = true;
		sym->constant = true;
		sym->value = read_num(str);
	}
	else if (token == T_CHAR){
		sym = Program_data::new_var(st_lit_int);
		sym->set = true;
		sym->constant = true;
		sym->value = read_char(str);
	}
	else if (token == T_STR){
		sym = Program_data::new_var(st_lit_str);
		sym->set = true;
		sym->constant = true;
		sym->str = Program_data::add_string(str);
	}
	else err_msg("Scanner::add_lit_sym(): brokdidit");
	
	return sym;
}

// The only thing that needs to happen here is convert escape sequences
//const char * Scanner::read_str(const char * str){
//	uint j=0;
//	
//	// FIXME: Need to handle the empty string case
//	
//	for(uint i=0; str[i] != 0; i++, j++){
//		if (str[i] == '\\'){
//			i++;
//			switch (str[i]){
//			case 'n' : str[j]='\n'; break;
//			case 'f' : str[j]='\f'; break;
//			case 'r' : str[j]='\r'; break;
//			case 't' : str[j]='\t'; break;
//			case 'v' : str[j]='\v'; break;
//			case '\\': str[j]='\\'; break;
//			case '"' : str[j]= '"'; break;
//			case '\n':         j--; break;
//			default: err_msg("Invalid escape sequence");
//			}
//		}
//		else str[j]=str[i];
//	}
//	
//	str[++j]=0;
//	
//	return str;
//}

umax Scanner::read_char(const char * str){
	if(Scanner::length() == 1) return str[0];
	else if (Scanner::length() == 2){
		if(str[0] != '\\') err_msg("funky multibyte character");
		switch (str[1]){
		case 'n' : return '\n';
		case 'f' : return '\f';
		case 'r' : return '\r';
		case 't' : return '\t';
		case 'v' : return '\v';
		case '\\': return '\\';
		//case '0' : return '\0'; // why would we need this?
		case '\'': return '\'';
		default  : err_msg("funky multibyte character");
		}
	}
	else err_msg("Character unusually large");
	
	return UMAX_MAX;
}

umax Scanner::read_num(const char * str) {
	umax num=0;
	const uint hexmax=16;
	const uint decmax=19;
	
	/*umax can hold up to UMAX_MAX
	2^64 = 10^x -> log_10 2^64 = x -> x=19.2659...
	2^64 = 16^y -> log_16 2^64 = y -> y=16
	*/
	
	// hex number
	if(str[1] == 'x' || str[1] == 'X'){ 
		if(Scanner::length() > hexmax +2){ // overflow
			err_msg("Number is too large");
			return UMAX_MAX;
		}
		
		for(uint i=2; i< Scanner::length(); i++){
			if (str[i]>= '0' && str[i]<='9')
				num = (num << 4) | (str[i] - '0');
			else if(str[i]>= 'A' && str[i]<='F')
				num = (num << 4) | (str[i] - 'A' + 10);
			else if(str[i]>= 'a' && str[i]<='f')
				num = (num << 4) | (str[i] - 'a' + 10);
			else err_msg("Internal: Scope_Stack::read_num(): hex");
		}
	}
	
	// decimal number
	else {
		if(Scanner::length() > decmax){ // overflow
			err_msg("Number is too large");
			return UMAX_MAX;
		}
		
		for(uint i=0; i< Scanner::length(); i++){
			num = (num * 10) + (str[i] - '0');
		}
	}
	
	return num;
}


