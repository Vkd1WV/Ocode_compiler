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
#include "token.h"


/******************************************************************************/
//                             SCANNER CLASS
/******************************************************************************/


class Scanner{
private:
	sym_pt  scan_sym;
	char *  scan_text;
	token_t scan_token;
	uint    lnum;
	
public:
	Scanner(char * filename);
	
	// Mutators
	void next_token(void);
	void resync    (void);
	
	// Accessors
	      sym_pt  get_sym  (void) const { return scan_sym  ; }
	const char *  get_text (void) const { return scan_text ; }
	      token_t get_token(void) const { return scan_token; }
	      uint    get_lnum (void) const { return lnum      ; }
};


#endif // _SCANNER_H


