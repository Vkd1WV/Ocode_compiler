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
void Primary   (void); // rcx

void Unary     (void); // returns in rcx. uses rdx

void Term      (void); // returns in rax. uses rdx, rcx
void Expression(void); // returns in rbx
void Equation  (void); // rsi
void Boolean   (void); // rsi

void Assignment(void);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


// Makes no changes to the output file
void Primary(void){
	sym_entry * sym_pt;
	
	switch (token){
	case T_NUM:
		fprintf(outfile, "\tmov rcx, 0%llxh\n", get_num());
		break;
	case T_CHAR:
		fprintf(outfile, "\tmov rcx, 0%llxh\n", get_char());
		break;
	case T_NAME:
		if(!( sym_pt=iview(symbol_table,get_name()) ))
			error("Undeclared symbol");
		
		fprintf(outfile, "\tmov rcx, %s\n", sym_pt->name);
		break;
	default:
		printf(
			"ERROR: could not match token: '0x%02x' on line %d to any rule\n",
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
	
	//return &symbol;
}

void Unary(void){ // value in rcx
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		emit_cmd("push rax");
		emit_cmd("push rbx");
		emit_cmd("push rsi");
		Boolean();
		emit_cmd("mov rcx, rdi");
		Match(T_CPAR);
		emit_cmd("pop rsi");
		emit_cmd("pop rbx");
		emit_cmd("pop rax");
		break;
	case T_DREF:
		get_token();
		Unary();
		emit_cmd("mov rdx, rcx");
		emit_cmd("mov rcx, [rdx]");
		break;
	case T_NOT:
		get_token();
		Unary();
		emit_cmd("cmp   rcx, 0");
		emit_cmd("movz  rcx, 1");
		emit_cmd("movnz rcx, 0");
		break;
	case T_INV:
		get_token();
		Unary();
		emit_cmd("not rcx");
		break;
	default:
		Primary();
	}
}

void Term(void){ // returns in rax. uses rdx,rcx
	Unary();
	emit_cmd("mov rax, rcx");
	
	while(token>=T_TIMES && token<=T_RSHFT){
		switch(token){
		case T_TIMES:
			get_token();
			Unary();
			emit_cmd("mul rcx");
			break;
		case T_DIV:
			get_token();
			Unary();
			emit_cmd("div rcx");
			break;
		case T_MODUL:
			get_token();
			Unary();
			emit_cmd("div rcx");
			emit_cmd("mov rax, rdx");
		case T_LSHFT:
			get_token();
			Unary();
			emit_cmd("shl rax"); // second op in CL implicit
			break;
		case T_RSHFT:
			get_token();
			Unary();
			emit_cmd("shr rax"); // second op in CL implicit
		}
	}
}

void Expression(void){ // returns in rbx
	Term();
	emit_cmd("mov rbx, rax");
	
	while (token>=T_PLUS && token<=T_RSHFT){
		token_t temp_token=token;
		get_token();
		Unary();
		
		switch (temp_token){
		case T_PLUS:  emit_cmd("add rax, rcx"); break;
		case T_MINUS: emit_cmd("sub rax, rcx"); break;
		case T_BAND:  emit_cmd("and rax, rcx"); break;
		case T_BOR:   emit_cmd("or  rax, rcx"); break;
		case T_BXOR:  emit_cmd("xor rax, rcx"); break;
		
		}
	}
}

void Equation(void){ // rsi
	Expression();
	emit_cmd("mov rsi, rbx");
	
	while (token>=T_EQ && token<=T_GTE){
		switch (token){
		case T_EQ:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("cmovnz rsi 0h");
			emit_cmd("cmovz rsi 1h");
			break;
		case T_NEQ:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("cmovnz rsi 1h");
			emit_cmd("cmovz rsi 0h");
			break;
		case T_LT:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("DO STUFF");
			break;
		case T_GT:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("DO STUFF");
			break;
		case T_LTE:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("DO STUFF");
			break;
		case T_GTE:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbx");
			emit_cmd("DO STUFF");
		}
	}
}

void Boolean(void){ // result in rsi
	char short_circ[UNQ_LABEL_SZ];
	strcpy(short_circ, new_label()); // make a label for short circuiting
	
	Equation();
	
	while(token == T_AND || token == T_OR){
		switch(token){
		case T_AND:
			emit_cmd("cmp rsi 0h");
			fprintf(outfile, "\tjz %s\n", short_circ);
			get_token();
			Equation();
			emit_cmd("cmp rsi 0h");
			fprintf(outfile, "\tjz %s\n", short_circ);
			break;
		case T_OR:
			emit_cmd("cmp rsi 0h");
			fprintf(outfile, "\tjnz %s\n", short_circ);
			get_token();
			Equation();
			emit_cmd("cmp rsi 0h");
			fprintf(outfile, "\tjnz %s\n", short_circ);
		}
	}
	emit_lbl(short_circ);
}

void Assignment(void){
	
	do {
		switch (token) {
		case T_ASS:
			get_token();
			Unary();
			emit_cmd("mov [rcx], rsi");
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
	emit_cmnt("An assignment Statement");
	Boolean();
	Assignment();
	Match(T_NL);
}


