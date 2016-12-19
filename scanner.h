/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _SCANNER_H
#define _SCANNER_H


/*	This file is just an interface between parser.c and scanner.c and doesn't
 *	need to be global.
 */

/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include "token.h"
#include "my_types.h"
#include "prog_data.h"


/******************************************************************************/
//                             SCANNER CLASS
/******************************************************************************/


class Scanner{
private:
	static sym_pt  scan_sym;
	static char *  scan_text;
	static token_t scan_token;
	static uint    txtlen;
	static uint    line_num;
	
	sym_pt       add_lit_sym(token_t token, const char * str);
	const char * read_str (const char * str);
	umax         read_char(const char * str);
	umax         read_num (const char * str);
	
public:
	 Scanner(char * filename);
	~Scanner(void);
	
	// Mutators
	void next_token (void     );
	void resync     (void     );
	void match_token(token_t t);
	void match_str  (char * s );
	
	// Accessors
	static sym_pt        sym   (void) { return scan_sym  ; }
	static token_t       token (void) { return scan_token; }
	static const char *  text  (void) { return scan_text ; }
	static uint          length(void) { return txtlen    ; }
	static uint          lnum  (void) { return line_num  ; }
};


#endif // _SCANNER_H


