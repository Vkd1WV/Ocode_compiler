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
const sym_entry * Primary   (void);

void Unary     (void); // returns in rcx. uses rdx

void Term      (void); // returns in rax. uses rdx, rcx
void Expression(void); // returns in rbx
void Equation  (void); // rsi
void Boolean   (void); // rsi

void Assign    (void);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


/*	If we implicitly dereference symbols we will need reference and dereference operators. Primary() will not be able to directly write the the output file because we may have to reference the label. We will not be able to decide to call a function name until Term().
*/

/*	In Omnicode Variable names are label names using them directly returns the address where the data is stored. Assignments require that address to be able change the data at that location. However, all other uses tend to need the data rather than its location and so should be dereferenced. The labels themselves obviously cannot be modifed.

@A + @B : C

*/

/*	Whatever the syntax, unary will need information about the type of symbol, 
	therefore Primary must make no changes to the registers and return only a 
	symbol entry.
*/

const sym_entry * Primary(void){
	static sym_entry symbol;
	sym_entry * sym_pt;
	
	symbol.name[0]   = '\0';
	symbol.code      = false;
	symbol.immediate = true;
	symbol.size      = void_t;
	
	switch (token){
	case T_NUM:
		symbol.value = get_num();
		return &symbol;
	case T_CHAR:
		symbol.value = get_char();
		return &symbol;
	case T_NAME:
		if(!( sym_pt=iview(symbol_table,get_name()) ))
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
}

/*	Since multible unaries can be stacked each should know the type of the
	subordinate operation.
*/

void Unary(void){ // value in rcx
	sym_entry * sym_pt;

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
		return;
	
	case T_DREF:
		get_token();
		
		sym_pt = Primary();
		if (sym_pt->code) error("cannot dereference a function");
		if (sym_pt->immediate) error("cannot dereference an immediate value");
		if (!sym_pt->dref) error("cannot dereference a void pointer");
		
		emit_cmd("mov rdx, rcx");
		emit_cmd("mov rcx, [rdx]");
		return;
	
	case T_NOT:
		get_token();
		
		sym_pt = Primary();
		if (sym_pt->code) error("cannot logical invert a function");
		
		if (sym_pt->immediate)
			fprintf(outfile, "\tmov rcx, %llu\n", sym_pt->value);
		else fprintf(outfile, "\tmov rcx, %s\n", sym_pt->name);
		
		emit_cmd("cmp   rcx, 0");
		emit_cmd("movz  rcx, 1");
		emit_cmd("movnz rcx, 0");
		return;
	
	case T_INV:
	
		get_token();
		
		sym_pt = Primary();
		if (sym_pt->code) error("cannot binary invert a function");
		
		if (sym_pt->immediate)
			fprintf(outfile, "\tmov rcx, %llu\n", sym_pt->value);
		else fprintf(outfile, "\tmov rcx, %s\n", sym_pt->name);
		
		emit_cmd("not rcx");
		return;
	
	default:
	
		sym_pt = Primary();
		
		if (sym_pt->immediate)
			fprintf(outfile, "\tmov rcx, %llu\n", sym_pt->value);
		else fprintf(outfile, "\tmov rcx, %s\n", sym_pt->name);
		
		return;
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
		case T_MOD:
			get_token();
			Unary();
			emit_cmd("div rcx");
			emit_cmd("mov rax, rdx");
		break;
		case T_EXP:
			get_token();
			Unary();
			emit_cmd("DO SOME THINGS");
		break;
		case T_LSHFT:
			get_token();
			Unary();
			emit_cmd("shl rax, cl");
		break;
		case T_RSHFT:
			get_token();
			Unary();
			emit_cmd("shr rax, cl");
		}
	}
}

void Expression(void){ // returns in rbx
	if(token == T_MINUS){
		get_token();
		Term();
		emit_cmd("neg rax");
	}
	else Term();
	
	emit_cmd("mov rbx, rax");
	
	while (token>=T_PLUS && token<=T_BXOR){
		token_t temp_token=token;
		get_token();
		Term();
		
		switch (temp_token){
		case T_PLUS:  emit_cmd("add rbx, rax"); break;
		case T_MINUS: emit_cmd("sub rbx, rax"); break;
		case T_BAND:  emit_cmd("and rbx, rax"); break;
		case T_BOR:   emit_cmd("or  rbx, rax"); break;
		case T_BXOR:  emit_cmd("xor rbx, rax"); break;
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
			emit_cmd("cmp    rsi, rbx");
			emit_cmd("cmove  rsi, 1h");
			emit_cmd("cmovne rsi, 0h");
			break;
		case T_NEQ:
			get_token();
			Expression();
			emit_cmd("cmp    rsi, rbx");
			emit_cmd("cmovne rsi, 1h");
			emit_cmd("cmove  rsi, 0h");
			break;
		case T_LT:
			get_token();
			Expression();
			emit_cmd("cmp    rsi, rbx");
			emit_cmd("cmovl  rsi, 1h");
			emit_cmd("cmovnl rsi, 0h");
			break;
		case T_GT:
			get_token();
			Expression();
			emit_cmd("cmp    rsi, rbx");
			emit_cmd("cmovg  rsi, 1h");
			emit_cmd("cmovng rsi, 0h");
			break;
		case T_LTE:
			get_token();
			Expression();
			emit_cmd("cmp     rsi, rbx");
			emit_cmd("cmovle  rsi, 1h");
			emit_cmd("cmovnle rsi, 0h");
			break;
		case T_GTE:
			get_token();
			Expression();
			emit_cmd("cmp     rsi, rbx");
			emit_cmd("cmovge  rsi, 1h");
			emit_cmd("cmovnge rsi, 0h");
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
			emit_cmd("cmp rsi, 0h");
			fprintf(outfile, "\tje %s\n", short_circ); // rsi = 0
			get_token();
			Equation();
			emit_cmd("cmp rsi, 0h");
			fprintf(outfile, "\tjz %s\n", short_circ); // else rsi != 0
			break;
		case T_OR:
			emit_cmd("cmp rsi, 0h");
			fprintf(outfile, "\tjne %s\n", short_circ);
			get_token();
			Equation();
			emit_cmd("cmp rsi, 0h");
			fprintf(outfile, "\tjne %s\n", short_circ);
		}
	}
	emit_lbl(short_circ);
}

void Assign(void){
	get_token();
	Unary();
	emit_cmd("mov [rcx], rsi");
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


void Assignment_Statement(void){ // result in rcx
	emit_cmnt("An assignment Statement");
	Boolean();
	while (token == T_ASS) Assign();
}


