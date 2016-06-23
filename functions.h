#ifndef _FUNCTIIONS_H
#define _FUNCTIIONS_H

// From Lexer
token_t yylex(void);

// From parse_expressions.c
void Assignment_Statement(void);
void Result    (void);

void Statement (uint lvl); // from parse_statements.c

// from parse.c
void Move(reg_t dest, regsz_t dsize, reg_t src, regsz_t ssize);
char* new_label(void);


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


#define TMP_ARR_SZE 100

static inline void error(const char* message){
	printf("ERROR: %s, on line %d\n", message, yylineno);
	exit(EXIT_FAILURE);
}

static inline void expected(const char* thing){
	char temp_array[TMP_ARR_SZE];
	sprintf(temp_array, "Expected %s, found 0x%x", thing, token);
	error(temp_array);
}

static inline void emit_cmnt(const char* message){
	fprintf(outfile, "\t; %s\n", message);
}

static inline void emit_cmd(const char* cmd){
	fprintf(outfile, "\t%s\n", cmd);
}

static inline void emit_lbl(char* lbl){
	fprintf(outfile, "\n%s:\n", lbl);
}

static inline void get_token(void){
	token=yylex();
}

static inline char* get_name(void){
	static char temp[NAME_MAX+1];
	
	if (token != T_NAME) expected("a name");
	strncpy(temp, yytext, NAME_MAX);
	get_token();
	return temp;
}

static inline umax get_char(void){
	static char temp;
	
	if (token != T_CHAR) expected("a character");
	temp = yytext[0];
	get_token();
	return temp;
}

static inline umax get_num(void){
	umax temp;
	
	if(token != T_NUM) expected("a number");
	temp=yynumber;
	get_token();
	return temp;
}

static inline void Match(token_t t){
	if(token == t) get_token();
	else{
		printf(
			"ERROR: Expected token: '0x%02x' but found token: '0x%02x' on line %d\n",
			t,
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
}


#endif // _FUNCTIIONS_H
