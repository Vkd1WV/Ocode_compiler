

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Label (void    );
void Jump  (void    );
void If    (uint lvl); // only control statements need to know the block_lvl
void While (uint lvl);
//void For   (uint lvl); for <range statement>


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
	
	condition = Boolean();
	if(condition->type == literal){}
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
	result = Boolean();
	emit_skp(skip_label, result);
	
	Statement(lvl);
	
	fprintf(outfile, "\tjmp\t%s\t#1\n", repeat_label);
	fprintf(outfile, "\nlbl %s # skip label\n" , skip_label);
	emit_cmnt("End of WHILE loop\n");
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
	
	else if (token >= T_8 && token <= T_TYPE) Decl_Symbol();
	
	else switch (token){
		// Operator declarations are in their own namespace
		case T_OPR:   Decl_Operator(   ); break;
		
		// Control Statements
		case T_LBL:   Label    (   ); break;
		case T_JMP:   Jump     (   ); break;
		case T_IF:    If       (lvl); break;
		case T_WHILE: While    (lvl); break;
		
		case T_NAME: // call sub or declare var of type_def
		default:
			Boolean();
			Match(T_NL);
	}
}

