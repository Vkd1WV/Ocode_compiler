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
#include <util/types.h>
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
	
	static sym_pt       add_lit_sym(token_t token, const char * str);
	static const char * read_str (const char * str);
	static umax         read_char(const char * str);
	static umax         read_num (const char * str);
	
public:
	 Scanner(const char * filename);
	~Scanner(void);
	
	// Mutators
	static void next_token(void);
	static void start_asm (void);
	
	// Accessors
	static inline sym_pt        sym   (void) { return scan_sym  ; }
	static inline token_t       token (void) { return scan_token; }
	static inline const char *  text  (void) { return scan_text ; }
	static inline uint          length(void) { return txtlen    ; }
	static inline uint          lnum  (void) { return line_num  ; }
};


#endif // _SCANNER_H


