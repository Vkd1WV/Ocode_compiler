#ifndef _FUNCTIIONS_H
#define _FUNCTIIONS_H

/******************************************************************************/
//                             GLOBAL PROTOTYPES
/******************************************************************************/


// From scanner.l
token_t yylex(void);

// From parse_declarations.c
void Decl_Operator(void);
void Decl_Symbol  (void);

// From parse_expressions.c
const sym_entry * Boolean(void);

// from parse_statements.c
void Statement   (uint lvl);

// From intermediate.c
const char * new_label              (void);
sym_entry  * new_var                (void);
void         Initialize_intermediate(void);
void Dump_symbols(void);

void emit_cmnt(const char* comment);
void emit_lbl(char* lbl);

void emit_quad(
	byte_code op,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
);


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


/************************ ERROR REPORTING & RECOVERY **************************/

static inline void parse_error(const char* message){
	printf("ERROR: %s, on line %d.\n", message, yylineno);
	while( (token = yylex()) != T_NL );
	printf("continuing...\n");
	longjmp(anewline, 1);
}

static inline void expected(const char* thing){
	char temp_array[ERR_ARR_SZ];
	sprintf(temp_array, "Expected %s, found %s", thing, yytext);
	parse_error(temp_array);
}

static inline void crit_error(const char* message){
	printf("CRITICAL ERROR: %s, on line %d.\n", message, yylineno);
	exit(EXIT_FAILURE);
}

/********************************* GETTERS ************************************/

static inline void get_token(void){
	token=yylex();
}

static inline char* get_name(void){
	static char name[NAME_MAX+1];
	
	if (token != T_NAME) expected("a name");
	strncpy(name, yytext, NAME_MAX);
	get_token();
	return name;
}

static inline umax get_num(void){
	umax num;
	
	if(token != T_NUM) expected("a number");
	num=yynumber;
	get_token();
	return num;
}

static inline void Match(token_t t){
	if(token == t) get_token();
	else{
		char temp_array[ERR_ARR_SZ];
		sprintf(temp_array, "0x%x", t);
		expected(temp_array);
	}
}

#endif // _FUNCTIIONS_H
