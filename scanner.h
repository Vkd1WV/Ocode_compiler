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
	
	void Bind(void); // bind the scan_text to a symbol
	
public:
	Scanner(void) { next_token(); } // initializate the lookahead token
	// Mutators
	void next_token(void);
	
	// Accessors
	      sym_pt  get_sym  (void) const { return scan_sym  ; }
	const char *  get_text (void) const { return scan_text ; }
	const token_t get_token(void) const { return scan_token; }
};


/******************************************************************************/
//                            GLOBAL STORAGE
/******************************************************************************/


//#ifdef _SCANNER_C
//	#define EXTERN const volatile
//#else
//	#define EXTERN extern
//#endif

//EXTERN sym_pt  scan_sym;
//EXTERN char *  scan_text;
//EXTERN token_t scan_token;


//#undef EXTERN


/******************************************************************************/
//                              PROTOTYPES
/******************************************************************************/

//void get_token(void);


#endif // _SCANNER_H


