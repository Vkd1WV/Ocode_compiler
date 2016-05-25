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
sym_entry* Primary   (void);

sym_entry* Unary     (void); // returns in rcx. uses rdx

void Term      (void); // returns in rax. uses rdx
void Expression(void); // returns in rbx
void Equation  (void); // rsi
void Boolean   (void); // rdi

void Assignment(void);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


sym_entry * Primary(void){
	static sym_entry symbol;
	sym_entry * sym_pt;
	
	switch (token){
/*	case T_OPAR:*/
/*		Match(T_OPAR);*/
/*		emit_cmd("push rax\n\tpush rbx\n\tpush rsi\n\tpush rdi");*/
/*		Boolean();*/
/*		Move(C, qword, DI, qword);*/
/*		Match(T_CPAR);*/
/*		emit_cmd("pop rdi\n\tpop rsi\n\tpop rbx\n\tpop rax");*/
/*		break;*/
	case T_NUM:
		strncpy(symbol.name, "", NAME_MAX);
		symbol.value = get_num();
		symbol.size  = qword;
		symbol.type  = 0;
		symbol.type += S_IMEDT;
		symbol.dref  = NULL;
		break;
	case T_CHAR:
		strncpy(symbol.name, "", NAME_MAX);
		symbol.value = get_char();
		symbol.size  = byte;
		symbol.type  = 0;
		symbol.type += S_IMEDT;
		symbol.dref  = NULL;
		break;
	case T_NAME:
		if((sym_pt=iview(symbol_table,get_name())) == NULL)
			error("Undeclared symbol");
		return sym_pt;
	default:
		printf(
			"ERROR: could not match token: '0x%02x' on line %d to any rule\n",
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
	
	return &symbol;
}

sym_entry* Unary(void){ // value in rcx
	sym_entry* sym_pt;
	
	switch (token){
	// inc and dec get sticky because they have implicit assignment
/*	case T_INC:*/
/*		get_token();*/
/*		sym_pt=Unary();*/
/*		*/
/*		if (sym_pt == NULL)*/
/*			error("Cannot increment a void data type. You probably used too many dereferences or need to use a cast.");*/
/*		if(sym_pt->flags & S_CONST) error("Cannot increment constant");*/
/*		*/
/*		if(sym_pt->flags & S_IMEDT) sym_pt->value++;*/
/*		// if it's not a constant or immediate, then it is a variable in memory*/
/*		else fprintf(outfile, "\tinc [%s]\n", sym_pt->name);*/
/*		// if type is a pointer should multiply by the sizeof*/
/*		*/
/*		return sym_pt;*/
/*	case T_DEC:*/
/*		get_token();*/
/*		sym_pt=Unary();*/
/*		*/
/*		if (sym_pt == NULL)*/
/*			error("Cannot decrement a void data type. You probably used too many dereferences or need to use a cast.");*/
/*		if(sym_pt->flags & S_CONST) error("Cannot decrement constant");*/
/*		*/
/*		if(sym_pt->flags & S_IMEDT) sym_pt->value--;*/
/*		// if it's not a constant or immediate, then it is a variable in memory*/
/*		else fprintf(outfile, "\tdec [%s]\n", sym_pt->name);*/
/*		*/
/*		return sym_pt;*/
	case T_DREF:
		get_token();
		sym_pt=Unary();
		
		if (sym_pt == NULL || *sym_pt->name == '\0'){ // pointers
			Move(D, qword, C, qword);
			emit_cmd("mov rcx, [rdx]");
		}
		else if (sym_pt->flags & S_CONST || sym_pt->flags & S_IMEDT) // consts
			fprintf(outfile, "\tmov rcx, [0%lxh]\n", sym_pt->value);
		else {
			fprintf(outfile, "\tmov rdx, [%s]\n", sym_pt->name); // vars
			fprintf(outfile, "\tmov rcx, [rdx]\n");
		}
		
		if (sym_pt == NULL)
			return NULL;
		return sym_pt->dref;
	case T_REF:
		get_token();
		sym_pt=Unary();
		
		if(sym_pt->flags & S_IMEDT || sym_pt->flags & S_CONST)
			error("cannot dereference constants");
		
		fprintf(outfile, "\tmov rcx, %s\n", sym_pt->name);
		
		return NULL;
	case T_NOT:
		get_token();
		sym_pt=Unary();
		
		emit_cmd("cmp rcx, 0h");
		emit_cmd("cmovz rcx, 01h");
		emit_cmd("cmovnz rcx, 0h");
		//FIXME: if larger than 1byte
		break;
	case T_INV:
		get_token();
		sym_pt=Unary();
		emit_cmd("not rcx");
		break;
	case T_MINUS:
		get_token();
		sym_pt=Unary();
		emit_cmd("neg rcx");
		break;
	default:
		return Primary();
	}
}

void Term(void){ // leaves result in rax
	Unary();
	Move(A, qword, C, qword);
	
	while(token >= T_TIMES && token <= T_RSHFT){
		switch(token){
/*		case T_TIMES:*/
/*			get_token();*/
/*			Unary();*/
/*			emit_cmd("imul rcx");*/
/*			break;*/
/*		case T_MODUL: // signed modulo*/
/*			get_token();*/
/*			Unary();*/
/*			emit_cmd("idiv rcx");*/
/*			Move(A, qword, D, qword); // remainder to acumulator*/
/*			break;*/
/*		case T_DIV:*/
/*			get_token();*/
/*			Unary();*/
/*			emit_cmd("div rcx");*/
/*			break;*/
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
	Term();
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
	
	while (token>=T_EQ && token<=T_GTE){
		switch (token){
		case T_EQ:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("cmovnz rsi 0h");
			break;
		case T_NEQ:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("cmovz rsi 0h");
			break;
		case T_LT:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("DO STUFF");
			break;
		case T_GT:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("DO STUFF");
			break;
		case T_LTE:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("DO STUFF");
			break;
		case T_GTE:
			get_token();
			Expression();
			emit_cmd("cmp rsi, rbi");
			emit_cmd("DO STUFF");
		}
	}
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


