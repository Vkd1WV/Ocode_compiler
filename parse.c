/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/************************ ERROR REPORTING & RECOVERY **************************/

static inline void parse_crit(sym_pt arg1, sym_pt arg2, const char * message){
	printf("PARSER CRITICAL ERROR: %s, on line %d.\n", message, yylineno);
	Print_sym(stderr, arg1);
	Print_sym(stderr, arg2);
	exit(EXIT_FAILURE);
}

static inline void parse_warn(const char * message){
	printf("WARNING: %s, on line %d.\n", message, yylineno);
}

static inline void Warn_comparison(sym_pt arg1, sym_pt arg2){
	if (arg1->type != arg2->type)
		parse_warn("Incompatible types in comparison");
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


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Decl_Symbol  (void);
void Decl_Operator(void);

void Decl_Word    (sym_pt templt);
void Decl_Pointer (sym_pt templt);
void Decl_Type    (sym_pt templt);
void Decl_Sub     (sym_pt new_sub);
void Decl_Fun     (sym_pt new_fun);

void Type_specifier(sym_pt templt_pt);

void Qualifier_list   (sym_pt templt);
void Initializer_list (sym_pt templt);
void Parameter_list   (sym_pt templt);

// expressions in order of precedence
static sym_pt Primary   (void);
static sym_pt Unary     (void);
static sym_pt Postfix   (void);
static sym_pt Term      (void);
static sym_pt Expression(void);
static sym_pt Equation  (void);
sym_pt Boolean          (void);

static sym_pt Assign(sym_pt target);


void Label (void    );
void Jump  (void    );
void If    (uint lvl); // only control statements need to know the block_lvl
void While (uint lvl);
//void For   (uint lvl); for <range statement>

void Statement (uint lvl);


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/


#include "parse_declarations.c"
#include "parse_expressions.c"
#include "parse_statements.c"


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void Parse(yuck_t * arg_pt){
	bool errors;
	
	// Set the infile
	if(arg_pt->nargs){
		yyin = fopen(*arg_pt->args, "r");
		if(!yyin) crit_error("No such file");
		sprintf(err_array, "Reading from: %s\n", *arg_pt->args);
		notice_msg(err_array);
	}
	else notice_msg("Reading from: stdin");
	
	// Set the debug file
	if (arg_pt->dashd_flag){
		char *dbgfile;
		
		dbgfile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_dbg)+1);
		if(! dbgfile) crit_error("Out of Memory");
		
		dbgfile = strcpy(dbgfile, *arg_pt->args);
		dbgfile = strncat(dbgfile, default_dbg, strlen(default_dbg));
		debug_fd = fopen(dbgfile, "w");
		
		fprintf(debug_fd,"#Omnicode Intermediate File\n");
		
		free(dbgfile);
	}
	
	get_token(); // Initialize the lookahead token
	emit_iop(add_name("_#GLOBAL_START"), I_NOP, NO_NAME, NULL, NULL, NULL);
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	emit_iop(NO_NAME, I_RTRN, NO_NAME, NULL, NULL, NULL);
	// TODO: add return statments to the global_inst_q
	
	// Close the infile
	fclose(yyin);
	
	// close the debug file
	if (debug_fd){
		
		info_msg("Producing debug file");
		Dump_symbols(debug_fd);
		Dump_iq(debug_fd);
		info_msg("Closing debug file");
		fclose(debug_fd);
		
	}
	
	if(errors){
		notice_msg("Errors were found. Exiting...");
		exit(EXIT_FAILURE);
	}
}


