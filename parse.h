#ifndef _PARSE_H
#define _PARSE_H

#include <string.h>
#include "tokens.h"
#include "utility.h"

/******************************************************************************/
//                        TYPE DEFINITIONS & LIMITS
/******************************************************************************/


typedef enum {Real, Protected, Virtual, SMM, Compatibility, Long} mode_t;
typedef enum { void_t, byte, word, dword, qword } regsz_t;
typedef enum{
	IMM,
	R0, R1, R2 , R3 , R4 , R5 , R6 , R7 ,
	//R8, R9, R10, R11, R12, R13, R14,
	R15,
	// x86
	A , B , C  , D  , SI , DI , BP ,
	// arm
	//A1, A2, A3 , A4 , V1 , V2 , V3 , V4 ,
	//V5, V6, V7 , V8 , WR , SB , SL , FP ,
	//IP, LR, PC,
	SP // stack pointer x86=R7, arm=R13
} reg_t;

#define UNQ_LABEL_SZ 12 // string lngth limit for compiler generated labels
#define NAME_MAX     64

typedef struct {
	char * name;
	uint64_t type;
}sym_entry;

// masks for sym_entry.type
#define S_CONST ((uint64_t) 1<<0)
#define S_STAT  ((uint64_t) 1<<1)
#define S_SIGN  ((uint64_t) 1<<2)
#define S_SZ    ((uint64_t) 7<<3) // 0 for void
#define S_REF   ((uint64_t) 7<<6) // levels of indirection


/******************************************************************************/
//                  PROTOTYPES & GLOBALS FROM SOURCE FILES
/******************************************************************************/


 // from parse_expressions.c
void Assignment_statement(void);
void Result    (void);

void Statement (uint lvl); // from parse_statements.c
extern char* yytext;   // from scanner.c

// from parse.c
extern token_t token; // global lookahead token
extern FILE* outfile;
void Block     (void);
void Move(reg_t dest, regsz_t dsize, reg_t src, regsz_t ssize);
char* new_label(void);


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/


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

static inline uint get_char(void){
	static char temp;
	
	if (token != T_CHAR) expected("a character");
	temp = yytext[0];
	get_token();
	return temp;
}

static inline int get_num(void){
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

#endif // _PARSE_H
