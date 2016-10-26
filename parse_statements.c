

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Label (void    );
void Jump  (void    );
void If    (uint lvl); // only control statements need to know the block_lvl
void While (uint lvl);
//void For   (uint lvl); for <range statement>

void Decl_word(void);
void Decl_Type(void);
void Decl_Sub (uint lvl);
void Decl_Fun (uint lvl);


/******************************************************************************/
//                           CONTROL STATEMENTS
/******************************************************************************/


void Label(void){
	Match(T_LBL);
	emit_lbl(get_name());
	Match(T_NL);
}

void Jump(void){
	Match(T_JMP);
	fprintf(outfile, "\tjmp\t%s\t#1\n", get_name());
	Match(T_NL);
}

void If(uint lvl){
	const sym_entry * condition;
	char if_label[UNQ_LABEL_SZ];
	char else_label[UNQ_LABEL_SZ];
	
	Match(T_IF);
	emit_cmnt("start of IF statement");
	strcpy(if_label, new_label());
	
	condition = Assignment_Statement();
//	emit_jmp()
	fprintf(outfile, "\tjz %s\n", if_label);
	
	Statement(lvl);
	
	if(token == T_ELSE){
		Match(T_ELSE);
		emit_cmnt("start of ELSE statement");
		strcpy(else_label, new_label());
		fprintf(outfile, "\tjmp %s\n", else_label);
		emit_lbl(if_label);
		
		Statement(lvl);
		
		fprintf(outfile, "%s: ; End of ELSE\n", else_label);
	}
	else
		fprintf(outfile, "%s:\n", if_label);
	emit_cmnt("End of IF Statement\n");
}

void While(uint lvl){
	char repeat_label[UNQ_LABEL_SZ], skip_label[UNQ_LABEL_SZ];
	const sym_entry * result;
	
	Match(T_WHILE);
	emit_cmnt("Start of WHILE loop");
	strcpy(repeat_label, new_label());
	strcpy(skip_label  , new_label());
	
	fprintf(outfile, "\nlbl %s # repeat label\n", repeat_label);
	result = Assignment_Statement();
	emit_skp(skip_label, result);
	
	Statement(lvl);
	
	fprintf(outfile, "\tjmp\t%s\t#1\n", repeat_label);
	fprintf(outfile, "\nlbl %s # skip label\n" , skip_label);
	emit_cmnt("End of WHILE loop\n");
}


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


// Declare a Variable
void Decl_word(void){
	sym_entry* new_symbol;
	
	//Initializations
	new_symbol=calloc(1, sizeof(sym_entry));
	if (!new_symbol) error("Out of memory");
	
	new_symbol->type = data;
	
	switch (token){
		case T_8:
			new_symbol->size=byte;
			break;
		case T_16:
			new_symbol->size=double_b;
			break;
		case T_32:
			new_symbol->size=quad_b;
			break;
		case T_64:
			new_symbol->size=octo_b;
			break;
		default: error("Internal compiler error at Decl_var()");
	}
	
	get_token();
	if (token == T_STATIC){
		new_symbol->stat = true;
		get_token();
	}
	else if (token == T_CONST){
		new_symbol->constant = true;
		get_token();
	}
	
	strncpy(new_symbol->name, get_name(), NAME_MAX);
	sort(global_symbols, new_symbol, new_symbol->name);
	//if(token == T_EQ) Assignment(new_symbol);
	Match(T_NL);
}

// Define a Datatype
void Decl_Type (void){}

// Declare a Subroutine
void Decl_Sub(uint lvl){
	sym_entry* new_sub;
	
	// Initializations
	new_sub=calloc(1, sizeof(sym_entry));
	if (!new_sub) error("Out of memory");
	
	new_sub->type  = subroutine;
	new_sub->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_sub->assembler = true;
	
	// Name
	strncpy(new_sub->name, get_name(), NAME_MAX);
	sort(global_symbols, new_sub, new_sub->name);
	
	// Parameter & Return Declarations
	
	Statement(lvl);
	
	// End statement
	Match(T_END);
	if (strcmp( new_sub->name, get_name() ))
		error("Miss-matched end statement");
	Match(T_NL);
}

// Declare a Function
void Decl_Fun (uint lvl){
	sym_entry* new_fun;
	
	// Initializations
	new_fun=calloc(1, sizeof(sym_entry));
	if (!new_fun) error("Out of memory");
	
	new_fun->type  = function;
	new_fun->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_fun->assembler = true;
	
	// Name
	strncpy(new_fun->name, get_name(), NAME_MAX);
	sort(global_symbols, new_fun, new_fun->name);
	
	// Parameter & Return Declarations
	
	Statement(lvl);
	
	// End statement
	Match(T_END);
	if (strcmp( new_fun->name, get_name() ))
		error("Miss-matched end statement");
	Match(T_NL);
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void Statement (uint lvl){ // any single line. always ends with NL
	
	if (token == T_NL){
		get_token();
		if(block_lvl <= lvl); // empty statement
		// Empty statements like this may occur as subordinates of other
		// control statements.
		
		else { // subordinate block
			lvl=block_lvl;
			fprintf(outfile, "\t# START block level %u\n", lvl);
			do {
				Statement(lvl);
			} while (token != T_EOF && block_lvl == lvl);
			if(block_lvl > lvl) error("Unexpected nested block");
			fprintf(outfile, "\t# END block level %u\n", lvl);
		}
	}
	else switch (token){
		case T_8:
		case T_16:
		case T_32:
		case T_64:    Decl_word(   ); break;
		case T_SUB:   Decl_Sub (lvl); break;
		case T_FUN:   Decl_Fun (lvl); break;
		case T_LBL:   Label    (   ); break;
		case T_JMP:   Jump     (   ); break;
		case T_IF:    If       (lvl); break;
		case T_WHILE: While    (lvl); break;
		default:
			Assignment_Statement();
			Match(T_NL);
	}
}

