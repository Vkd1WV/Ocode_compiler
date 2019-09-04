/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/




#include "errors.h"
#include "proto.h"
#include "scope.h"
#include "parse.h"

#include <stdlib.h>
#include <string.h>
#include <setjmp.h>


/******************************************************************************/
//                      PARSER MODULE TYPE DEFINITIONS
/******************************************************************************/





/******************************************************************************/
//                         PARSER MODULE VARIABLES
/******************************************************************************/


//static Program_data      * intermed; // used by pop_scope()
//static Scanner           * scanner;
//static Instruction_Queue * parse_q; ///< pointer to the current scope inst_q


// the break and continue labels for the smallest current looping construct
str_dx break_this = NO_NAME, continue_this = NO_NAME;


jmp_buf anewline;  ///< to facilitate error recovery


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/************************ ERROR REPORTING & RECOVERY **************************/

static inline void parse_crit(sym_pt arg1, sym_pt arg2, const char * message){
	msg_print(NULL,V_ERROR,"PARSER CRITICAL ERROR: %s, on line %d.\n",
		message,
		Scanner::lnum()
	);
	Print_sym(stderr, arg1);
	Print_sym(stderr, arg2);
	
	exit(EXIT_FAILURE);
}

static inline void parse_error(const char * message){
	msg_print(NULL,V_ERROR, "CODE ERROR: %s, on line %d.\n",
		message,
		Scanner::lnum()
	);
	while(Scanner::token() != T_EOF && Scanner::token() != T_NL)
		Scanner::next_token();
	msg_print(NULL,V_ERROR, "continuing...\n");
	longjmp(anewline, 1);
}

static inline void parse_warn(const char * message){
	msg_print(NULL,V_WARN, "%s, on line %d.\n",
		message,
		Scanner::lnum()
	);
}

//static inline void Warn_comparison(sym_pt arg1, sym_pt arg2){
//	if(verbosity >= V_WARN && arg1->type != arg2->type)
//		parse_warn("Incompatible types in comparison");
//}

static inline void expected(const char* thing){
	sprintf(err_array, "Expected '%s', found '%s'",
		thing,
		Scanner::text()
	);
	parse_error(err_array);
}



/********************************* GETTERS ************************************/

static inline bool match_string(const char * string){
	if ( strcmp(string, Scanner::text()) ){
		expected(string);
	}
	Scanner::next_token();
	return true;
}

static inline void match_token(token_t t){
	if(Scanner::token() == t) Scanner::next_token();
	else expected(token_dex[t]);
}

//static inline char * get_name(void){
//	static char * buffer;
//	static size_t buf_lngth;
//	
//	if (Scanner::token() != T_NAME) expected("a name");
//	
//	// size the buffer if necessary
//	if(!buffer){
//		buffer = (char*)malloc(Scanner::length()+1);
//		buf_lngth = Scanner::length()+1;
//	}
//	else if (Scanner::length()+1 > buf_lngth){
//		buffer = (char*)realloc(buffer, Scanner::length()+1);
//		buf_lngth = Scanner::length()+1;
//	}
//	
//	if (!buffer) crit_error("Out of Memory");
//	
//	strncpy(buffer, Scanner::text(), Scanner::length());
//	
//	Scanner::next_token();
//	return buffer;
//}


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


//sym_pt get_scope(void);

// Emmiters
/*str_dx add_name (char * name);*/
/*str_dx new_label(void); ///< get a new unique label*/
/*sym_pt  new_var  (sym_type);  ///< Create a unique temporary symbol*/
/*void    emit_iop (*/
/*	str_dx      label,*/
/*	op_code      op,*/
/*	str_dx      target,*/
/*	const sym_pt out,*/
/*	const sym_pt left,*/
/*	const sym_pt right*/
/*);*/

/*// Declarations*/
//void Decl_Symbol  (void    );
/*void Decl_Operator(uint lvl);*/
/*void Decl_Type    (uint lvl);*/
/*void Decl_Sub     (uint lvl);*/
/*void Decl_Fun     (uint lvl);*/

/*void Decl_Custom  (sym_pt templt);*/
/*void Decl_Word    (sym_pt templt);*/
/*void Decl_Pointer (sym_pt templt);*/

//void Type_specifier(sym_pt templt_pt);

//bool Declaration(uint lvl);
//void Decl_list(uint lvl);

// Declarations
void Decl_Storage (void);
void Decl_Sub     (void);
void Decl_Fun     (void);
void Decl_Type    (void);
void Decl_Operator(void);
void Decl_Implicit(void);

// expressions
static sym_pt Boolean          (void);
static sym_pt Assign(sym_pt target);

/*// Statements*/
void Statement (void);


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


#include "parse_declarations.cpp"
#include "parse_expressions.cpp"
#include "parse_statements.cpp"


/******************************************************************************/
//                              PUBLIC FUNCTION
/******************************************************************************/


bool Parse(const char * infile){
	int errors;
	
	msg_print(logfile,V_TRACE,"Parse(): start");
	
	// set various global pointers
//	intermed = d;
//	scanner = s;
	
	
	// Initialize the scope stack
	Scope_Stack scope;
	scope.push(GLOBAL_SCOPE);
	
	// Initialize the scanner
	Scanner scan(infile);
	
	
	/*
		Program
			: 0 <= declarations
			: 1 <= statements
			;
		Library
			: 1 <= declarations
			;
	*/
	
	scope.emit_lbl( Program_data::add_string(START_LBL), NULL);
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	if(scan.token() == T_EOF) crit_error("Cannot compile an empty file");
	
	do{
		Statement();
	} while (scan.token() != T_EOF);
	
	// FIXME: this should be a return 0. ret is a unary operator.
	scope.emit_op(I_RTRN, NULL, Program_data::unq_sym(st_lit_int), NULL);
	
	
	
	if(errors){
		msg_print(NULL, V_WARN, "Parse(): errors were found");
		return true;
	}
	else{
		Optomize(scope.pop());
		msg_print(logfile,V_TRACE,"Parse(): stop");
		return false;
	}
}


