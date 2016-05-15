/******************************************************************************/
// parse_expressions.c
// handles expression parsing and variable assignment of the omega language
//
// Ammon Dodson
// Spring 2016
/******************************************************************************/

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


// expressions in order of precedence
void Primary   (void); // returns in rcx.
void Unary     (void); // returns in rcx. uses rdx

void Term      (void); // returns in rax. uses rdx
void Expression(void); // returns in rbx
void Equation  (void); // rsi
void Boolean   (void); // rdi

void Assignment(void);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


void Primary(void){
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		emit_cmd("push rax\n\tpush rbx\n\tpush rsi\n\tpush rdi");
		Boolean();
		Move(C, qword, DI, qword);
		Match(T_CPAR);
		emit_cmd("pop rdi\n\tpop rsi\n\tpop rbx\n\tpop rax");
		break;
	case T_NUM:
		fprintf(outfile, "\tmov rcx, 0%xh\n", get_num());
		break;
	case T_CHAR:
		fprintf(outfile, "\tmov rcx, 0%xh\n", get_char());
		break;
	case T_NAME:
		fprintf(outfile, "\tmov rcx, %s\n", get_name());
		// FIXME: need a symbol table
		break;
	default:
		printf(
			"ERROR: could not match token: '0x%02x' on line %d to any rule\n",
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
}

void Unary(void){
	switch (token){
	case T_INC:
		get_token();
		Primary();
		emit_cmd("inc rcx");
		break;
	case T_DEC:
		get_token();
		Primary();
		emit_cmd("dec rcx");
		break;
	case T_DREF:
		get_token();
		Primary();
		Move(D, qword, C, qword);
		emit_cmd("mov rcx, [rdx]");
		break;
	case T_REF:
		get_token();
		Primary();
		emit_cmd("DO SOME THINGS");
		break;
	case T_NOT:
		get_token();
		Primary();
		emit_cmd("cmovz rcx, 01h");
		emit_cmd("cmovnz rcx, 0h");
		//FIXME: mov does not set flags
		break;
	case T_INV:
		get_token();
		Primary();
		emit_cmd("not rcx");
		break;
	default:
		Primary();
	}
}

void Term(void){ // leaves result in rax
	Unary();
	Move(A, qword, C, qword);
	
	while(token >= T_TIMES && token <= T_RSHFT){
		switch(token){
		case T_TIMES:
			get_token();
			Unary();
			emit_cmd("imul rcx");
			break;
		case T_MODUL:
			get_token();
			Unary();
			emit_cmd("idiv rcx");
			Move(A, qword, D, qword); // remainder to acumulator
			break;
		case T_DIV:
			get_token();
			Unary();
			emit_cmd("div rcx");
			break;
		case T_LSHFT:
			get_token();
			Unary();
			fprintf(outfile, "\tDO SOME THINGS\n");
			break;
		case T_RSHFT:
			get_token();
			Unary();
			fprintf(outfile, "\tDO SOME THINGS\n");
		}
	}
}

void Expression(void){ // leaves result in rbx
	if (token == T_MINUS){ // handle negative numbers
		Match(T_MINUS);
		Term();
		emit_cmd("neg rax");
	}
	else Term();
	Move(B, qword, A, qword);
	
	while (token>=T_PLUS && token<=T_BXOR){
		switch (token){
		case T_PLUS:
			get_token();
			Term();
			emit_cmd("add rbx, rax");
			break;
		case T_MINUS:
			get_token();
			Term();
			emit_cmd("sub rbx, rax");
			break;
		case T_BAND:
			get_token();
			Term();
			emit_cmd("and rbx, rax");
			break;
		case T_BOR:
			get_token();
			Term();
			emit_cmd("or rbx, rax");
			break;
		case T_BXOR:
			get_token();
			Term();
			emit_cmd("xor rbx, rax");
		}
	}
}

void Equation(void){ // result in rsi
	Expression();
	Move(SI, qword, B, qword);
}

void Boolean(void){ // result in rdi
	char short_circ[UNQ_LABEL_SZ];
	strcpy(short_circ, new_label()); // make a label for short circuiting
	
	Equation();
	Move(DI, qword, SI, qword);
	
	while(token == T_AND || token == T_OR){
		switch(token){
		case T_AND:
			get_token();
			Equation();
			fprintf(outfile, "\tDO SOME THINGS\n");
			break;
		case T_OR:
			get_token();
			Equation();
			fprintf(outfile, "\tadd    rdi, rsi\n");
			fprintf(outfile, "\tcmovnz rdi, 01h\n");
		}
	}
	emit_lbl(short_circ);
}

void Assignment(void){
	do {
		switch (token) {
		case T_ASS:
			get_token();
			Move(A, qword, DI, qword);
			Unary();
			emit_cmd("DO SOME THINGS");
			break;
		default:
			expected("an assignment");
		}
	} while (token == T_ASS);
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


void Result(void){
	Boolean();
	if(token == T_ASS) Assignment();
}

void Assignment_statement(void){ // result in rcx
	Boolean();
	Assignment();
	Match(T_NL);
}


