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


// Makes no changes to the output file
sym_entry * Primary(void){
	static sym_entry symbol;
	sym_entry * sym_pt;
	
	switch (token){
	case T_NUM:
		strncpy(symbol.name, "", NAME_MAX);
		symbol.value  = get_num();
		symbol.size   = qword;
		symbol.type  = 0;
		symbol.type += S_IMEDT;
		symbol.dref   = NULL;
		break;
	case T_CHAR:
		strncpy(symbol.name, "", NAME_MAX);
		symbol.value  = get_char();
		symbol.size   = byte;
		symbol.type  = 0;
		symbol.type += S_IMEDT;
		symbol.dref   = NULL;
		break;
	case T_NAME:
		if((sym_pt=iview(symbol_table,get_name())) == NULL)
			error("Undeclared symbol");
		
		// if it's a constant we make a local copy we can change.
		if(sym_pt->type & S_CONST) memcpy(&symbol, sym_pt, sizeof(sym_entry));
		else return sym_pt;
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

sym_entry* Unary(void){ // value in rbx
	sym_entry* sym_pt;
	
	switch (token){
/*	case T_OPAR:*/
/*		Match(T_OPAR);*/
/*		emit_cmd("push rax\n\tpush rbx\n\tpush rsi\n\tpush rdi");*/
/*		Boolean();*/
/*		Move(C, qword, DI, qword);*/
/*		Match(T_CPAR);*/
/*		emit_cmd("pop rdi\n\tpop rsi\n\tpop rbx\n\tpop rax");*/
/*		break;*/
	case T_DREF:
		get_token();
		sym_pt=Unary();
		
		if (sym_pt->type & S_IMEDT)
			fprintf(outfile, "\tmov rbx, [0%lxh]\n", sym_pt->value);
		else if(sym_pt->dref == NULL) {
			comment("WARNING: dereferencing unknown pointer type");
		}
		else
			fprintf(outfile, "\tmov rdx, [%s]\n", sym_pt->name); // vars
			fprintf(outfile, "\tmov rcx, [rdx]\n");
		}
		
		if (sym_pt == NULL)
			return NULL;
		return sym_pt->dref;
	case T_REF:
		get_token();
		sym_pt=Primary();
		
		if(sym_pt->type & S_IMEDT || sym_pt->type & S_CONST)
			error("cannot create references to constants");
		if(sym_pt->type & S_REF)
			error("cannot create a double reference. References are stored as immediates, or constants until they are assigned.");
		
		sym_pt->type |= S_REF;
		fprintf(outfile, "\tmov rbx, %s\n", sym_pt->name);
		
		return sym_pt;
	case T_NOT:
		get_token();
		sym_pt=Unary();
		
		if(sym_pt->type & S_IMEDT || sym_pt->type & S_CONST){
			sym_pt->value = !(sym_pt->value);
			return sym_pt;
		}
		
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
	default:
		return Primary();
	}
}

void Expression(void){ // leaves result in accumulator
	Unary();
	Move(A, qword, B, qword);
	
	while (token>=T_PLUS && token<=T_RSHFT){
		token_t temp_token=token;
		get_token();
		Unary();
		
		switch (temp_token){
		case T_PLUS:  emit_cmd("add rax, rbx"); break;
		case T_MINUS: emit_cmd("sub rax, rbx"); break;
		case T_BAND:  emit_cmd("and rax, rbx"); break;
		case T_BOR:   emit_cmd("or  rax, rbx"); break;
		case T_BXOR:  emit_cmd("xor rax, rbx"); break;
		case T_LSHFT:
			Move(C, byte, B, byte);
			emit_cmd("shl rax");
			break;
		case T_RSHFT:
			Move(C, byte, B, byte);
			emit_cmd("shr rax");
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


