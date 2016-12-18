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


#include "prog_data.h"
#include "scope.h"
#include "token.h"


/******************************************************************************/
//                             SCANNER CLASS
/******************************************************************************/


class Scanner{
private:
	sym_pt  scan_sym;
	char *  scan_text;
	token_t scan_token;
	uint    line_num;
	
public:
	Scanner(char * filename);
	
	// Mutators
	void next_token (void     );
	void resync     (void     );
	void match_token(token_t t);
	void match_str  (char * s );
	
	// Accessors
	      sym_pt  sym  (void) const { return scan_sym  ; }
	const char *  text (void) const { return scan_text ; }
	      token_t token(void) const { return scan_token; }
	      uint    lnum (void) const { return line_num  ; }
};


#endif // _SCANNER_H


