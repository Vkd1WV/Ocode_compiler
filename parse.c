/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"
#include "scanner.h"


/******************************************************************************/
//                   GLOBAL VARIABLES IN THE PARSER MODULE
/******************************************************************************/


//token_t token;     ///< global lookahead token

static DS     symbols;       ///< symbol table
static DS     global_inst_q; ///< a global instruction queue
static DS     sub_inst_q;    ///< an instruction queue for subroutines
       char * name_array;    ///< dynamic array for symbol and label names

static DS scope_stack; ///< keep track of the current scope


// string length limit for unique compiler generated labels
// sufficiently large for 32-bit numbers in decimal and then some.
#define UNQ_NAME_SZ 16
//#define COLLISION_CHAR (char)'#'

// the break and continue labels for the smallest current looping construct
name_dx break_this = NO_NAME, continue_this = NO_NAME;


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
			yylineno
		);
	Print_sym(stderr, arg1);
	Print_sym(stderr, arg2);
	}
	
	exit(EXIT_FAILURE);
}

static inline void parse_warn(const char * message){
	if(verbosity >= V_WARN)
		fprintf(stderr, "WARNING: %s, on line %d.\n", message, yylineno);
}

static inline void Warn_comparison(sym_pt arg1, sym_pt arg2){
	if(verbosity >= V_WARN && arg1->type != arg2->type)
		parse_warn("Incompatible types in comparison");
}

static inline void get_token(void){
	token=yylex();
}

static inline void expected(const char* thing){
	char temp_array[ERR_ARR_SZ];
	sprintf(temp_array, "Expected %s, found %s", thing, yytext);
	parse_error(temp_array);
}

static inline void debug_sym(const char * message, sym_pt sym){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, yylineno);
		Print_sym(stderr, sym);
	}
}

static inline void debug_iop(const char * message, icmd * iop){
	if (verbosity >= V_DEBUG){
		fprintf(stderr, "%s, on line %4d: ", message, yylineno);
		Print_icmd(stderr, iop);
	}
}

/********************************* GETTERS ************************************/

static inline umax get_num(void){
	umax num;
	
	if(token != T_NUM) expected("a number");
	num=yynumber;
	get_token();
	return num;
}

static inline bool Match_name(name_dx dx){
	if ( token != T_NAME || strcmp(name_array+dx, yytext) ){
		expected(name_array+dx);
	}
	get_token();
	return true;
}

static inline void Match(token_t t){
	if(token == t) get_token();
	else{
		char temp_array[ERR_ARR_SZ];
		sprintf(temp_array, "0x%x", t);
		expected(temp_array);
	}
}

static inline char * get_name(void){
	static char * buffer;
	static size_t buf_lngth;
	size_t lngth;
	
	if (token != T_NAME) expected("a name");
	
	lngth = strlen(yytext)+1; // +1 for the \0
	
	// size the buffer if necessary
	if(!buffer){
		buffer = malloc(lngth);
		buf_lngth = lngth;
	}
	else if (lngth > buf_lngth){
		buffer = realloc(buffer, lngth);
		buf_lngth = lngth;
	}
	
	if (!buffer) crit_error("Out of Memory");
	
	strncpy(buffer, yytext, lngth);
	
	get_token();
	return buffer;
}

/**************************** SCOPE FUNCTIONS *********************************/

// get the current scope prefix
static inline const char * scope_prefix(void){
	sym_pt * sym;
	if(( sym = (sym_pt*)DS_first(scope_stack) ))
		return dx_to_name((*sym)->name);
	else return "";
}

/*
This function returns the correct symbol for the given name in the current scope
*/
static inline sym_pt Bind(char * name){
	char * buffer, * prefix;
	sym_pt sym=NULL, scope_sym, *thing;
	size_t name_l;
	
	if(( thing = (sym_pt*)DS_first(scope_stack) )){
		scope_sym = *thing;
		prefix = dx_to_name(scope_sym->name);
		name_l = strlen(prefix) + strlen(name) + 1;
		buffer = (char*)malloc(name_l);
		
		do {
			scope_sym = *thing;
			prefix = dx_to_name(scope_sym->name);
			strncpy(buffer, prefix, strlen(prefix));
			strncat(buffer, name, strlen(name));
			
			if(( sym = DS_find(symbols, buffer) )) break;
		} while (( thing = (sym_pt*)DS_next(scope_stack) ));
	}
	
	// if we didn't find anything, check the global scope
	if(!sym) sym = DS_find(symbols, name);
	
	return sym;
}

static inline void push_scope(sym_pt sym){
	DS_push(scope_stack, &sym);
}

static inline void pop_scope(void){
	DS_pop(scope_stack);
}


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


//sym_pt get_scope(void);

// Emmiters
name_dx add_name (char * name);
name_dx new_label(void); ///< get a new unique label
sym_pt  new_var  (sym_type);  ///< Create a unique temporary symbol
void    emit_iop (
	name_dx      label,
	op_code      op,
	name_dx      target,
	const sym_pt out,
	const sym_pt left,
	const sym_pt right
);

// Declarations
void Decl_Symbol  (void    );
void Decl_Operator(uint lvl);
void Decl_Type    (uint lvl);
void Decl_Sub     (uint lvl);
void Decl_Fun     (uint lvl);

void Decl_Custom  (sym_pt templt);
void Decl_Word    (sym_pt templt);
void Decl_Pointer (sym_pt templt);

void Type_specifier(sym_pt templt_pt);

void Decl_list(uint lvl);

// expressions in order of precedence
static sym_pt Primary   (void);
static sym_pt Unary     (void);
static sym_pt Postfix   (void);
static sym_pt Term      (void);
static sym_pt Expression(void);
static sym_pt Equation  (void);
sym_pt Boolean          (void);

static sym_pt Assign(sym_pt target);

// Statements
void Label   (void);
void Jump    (void);
void Break   (void);
void Continue(void);
void Return  (void);
void Call_sub(sym_pt sub);
void If      (uint lvl); // only control statements need to know the block_lvl
void While   (uint lvl);
void Do      (uint lvl);
void For     (uint lvl); //for <range statement>
void Switch  (uint lvl);

void Statement (uint lvl);


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


#include "parse_declarations.c"
#include "parse_expressions.c"
#include "parse_statements.c"
#include "emitters.c"


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


bool Parse(Program_data * data, char * infilename){
	bool errors;
	
	// set various global pointers
	symbols       = data->symbols;
	global_inst_q = data->main_q;
	sub_inst_q    = data->sub_q;
	
	// Initialize the scope stack
	scope_stack = (DS)DS_new_list(sizeof(sym_pt));
	
	// set the infile
	if(infilename){
		sprintf(err_array, "Reading from: %s", infilename);
		info_msg(err_array);
		yyin = fopen(infilename, "r");
		if(!yyin) {
			sprintf(err_array, "No such file: %s", infilename);
			crit_error(err_array);
		}
	}
	else info_msg("Reading from: stdin");
	
	get_token(); // Initialize the lookahead token
	
	emit_iop(add_name(START_LBL), I_NOP, NO_NAME, NULL, NULL, NULL);
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	// get global declarations
	Decl_list(0);
	
	while (token != T_EOF) {
		Statement(0);
	}
	
	// Close the infile
	fclose(yyin);
	
	data->names = name_array;
	
	if(errors){
		warn_msg("Parse(): errors were found");
		return true;
	}
	
	emit_iop(NO_NAME, I_RTRN, NO_NAME, NULL, NULL, NULL);
	
	DS_delete(scope_stack);
	
	return false;
}


