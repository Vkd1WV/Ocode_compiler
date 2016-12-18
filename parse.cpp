/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "scanner.h"
#include "prog_data.h"


/******************************************************************************/
//                      PARSER MODULE TYPE DEFINITIONS
/******************************************************************************/





/******************************************************************************/
//                         PARSER MODULE VARIABLES
/******************************************************************************/


static Program_data      * intermed; // used by pop_scope()
static Scanner           * scanner;
static Instruction_Queue * parse_q; ///< pointer to the current scope inst_q

//#define COLLISION_CHAR (char)'#'

// the break and continue labels for the smallest current looping construct
str_dx break_this = NO_NAME, continue_this = NO_NAME;


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/************************ ERROR REPORTING & RECOVERY **************************/

static inline void parse_crit(sym_pt arg1, sym_pt arg2, const char * message){
	if (verbosity >= V_ERROR){
		fprintf(
			stderr,
			"PARSER CRITICAL ERROR: %s, on line %d.\n",
			message,
			scanner->get_lnum()
		);
		Print_sym(stderr, arg1, intermed);
		Print_sym(stderr, arg2, intermed);
	}
	
	exit(EXIT_FAILURE);
}

static inline void parse_error(const char * message){
	fprintf(stderr, "CODE ERROR: %s, on line %d.\n",
		message,
		scanner->get_lnum()
	);
	scanner->resync();
	fprintf(stderr, "continuing...\n");
	longjmp(anewline, 1);
}

static inline void parse_warn(const char * message){
	if(verbosity >= V_WARN)
		fprintf(stderr, "WARNING: %s, on line %d.\n",
			message,
			scanner->get_lnum()
		);
}

static inline void Warn_comparison(sym_pt arg1, sym_pt arg2){
	if(verbosity >= V_WARN && arg1->type != arg2->type)
		parse_warn("Incompatible types in comparison");
}

static inline void expected(const char* thing){
	char temp_array[ERR_ARR_SZ];
	
	sprintf(temp_array, "Expected '%s', found '%s'",
		thing,
		scanner->get_text()
	);
	parse_error(temp_array);
}

static inline void debug_sym(const char * message, sym_pt sym){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, scanner->get_lnum());
		Print_sym(stderr, sym, intermed);
	}
}

static inline void debug_iop(const char * message, iop_pt iop){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, scanner->get_lnum());
		Print_iop(stderr, iop, intermed);
	}
}

/********************************* GETTERS ************************************/

//static inline umax get_num(void){
//	umax num;
//	
//	if(token != T_NUM) expected("a number");
//	num=yynumber;
//	get_token();
//	return num;
//}

//static inline bool Match_string(char * string){
//	if ( token != T_NAME || strcmp(string, yytext) ){
//		expected(string);
//	}
//	get_token();
//	return true;
//}

//static inline void Match(token_t t){
//	if(token == t) get_token();
//	else expected(token_dex[t]);
//}

//static inline char * get_name(void){
//	static char * buffer;
//	static size_t buf_lngth;
//	size_t lngth;
//	
//	if (token != T_NAME) expected("a name");
//	
//	lngth = strlen(yytext)+1; // +1 for the \0
//	
//	// size the buffer if necessary
//	if(!buffer){
//		buffer = malloc(lngth);
//		buf_lngth = lngth;
//	}
//	else if (lngth > buf_lngth){
//		buffer = realloc(buffer, lngth);
//		buf_lngth = lngth;
//	}
//	
//	if (!buffer) crit_error("Out of Memory");
//	
//	strncpy(buffer, yytext, lngth);
//	
//	get_token();
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

void Type_specifier(sym_pt templt_pt);

bool Declaration(uint lvl);
void Decl_list(uint lvl);

// expressions
static sym_pt Boolean          (void);
static sym_pt Assign(sym_pt target);

/*// Statements*/
void Statement (uint lvl);


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


#include "parse_declarations.c"
#include "parse_expressions.c"
#include "parse_statements.c"


/******************************************************************************/
//                              PUBLIC FUNCTION
/******************************************************************************/


bool Parse(Program_data * d, Scanner * s){
	int errors;
	
	// set various global pointers
	intermed = d;
	scanner = s;
	
	// Initialize the scope stack
	scope_stack = (DS)DS_new_list(sizeof(prg_blk));
	
	
	
	get_token(); // Initialize the lookahead token
	push_scope(NULL); // initialize the scope stack
	
	
	/*
		Program
			: 0 <= declarations
			: 1 <= statements
			;
		Library
			: 1 <= declarations
			;
	*/
	
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	// get global declarations
	Decl_list(0);
	
	emit_iop(add_name(START_LBL), I_NOP, NO_NAME, NULL, NULL, NULL);
	
	errors+=setjmp(anewline); // once global declarations are finished
	
	while (token != T_EOF) {
		Statement(0);
	}
	
	// Close the infile
	fclose(yyin);
	yyin = NULL;
	
	emit_iop(NO_NAME, I_RTRN, NO_NAME, NULL, NULL, NULL);
	pop_scope();
	data->names = name_array;
	DS_delete(scope_stack);
	
	if(errors){
		warn_msg("Parse(): errors were found");
		return true;
	}
	else return false;
}


