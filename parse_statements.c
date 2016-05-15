

#include "parse.h"
/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Label     (void);
void Jump      (void);
void If        (uint lvl); // only control statements need to know the block_lvl
void While     (uint lvl);


/******************************************************************************/
//                             PRIVATE FUNCTIONS
/******************************************************************************/

void If(uint lvl){
	char if_label[UNQ_LABEL_SZ];
	char else_label[UNQ_LABEL_SZ];
	
	Match(T_IF);
	emit_cmnt("start of IF statement");
	strcpy(if_label, new_label());
	
	Result();
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
	
	Match(T_WHILE);
	emit_cmnt("Start of WHILE loop");
	strcpy(repeat_label, new_label());
	strcpy(skip_label  , new_label());
	
	fprintf(outfile, "%s: ; repeat label\n", repeat_label);
	Result();
	fprintf(outfile, "\tjz %s ; skip jump\n", skip_label);
	
	Statement(lvl);
	
	fprintf(outfile, "\tjmp %s ; repeat\n", repeat_label);
	fprintf(outfile, "%s: ; skip label\n" , skip_label);
	emit_cmnt("End of WHILE loop\n");
}

void Label(void){
	Match(T_LBL);
	emit_lbl(get_name());
	Match(T_NL);
}

void Jump(void){
	Match(T_JMP);
	fprintf(outfile, "\tjmp %s\n", get_name());
	Match(T_NL);
}




/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


/*void Block(void){ // a block needs to be a type of statement*/
/*	uint lvl=block_lvl;*/
/*	*/
/*	Match(T_NL);*/
/*	if(block_lvl <= lvl) expected("indented block");*/
/*	lvl=block_lvl;*/
/*	printf("\t; starting block level %u\n", lvl);*/
/*	*/
/*	do {*/
/*		Statement();*/
/*	} while (token != T_EOF && block_lvl == lvl);*/
/*	*/
/*//	if(token == T_EOF && !lvl)*/
/*//		error("End of File reached before end of current block");*/
/*	if(block_lvl > lvl)*/
/*		error("Unexpected nested block");*/
/*	*/
/*	printf("\t; end of block level %u\n", lvl);*/
/*}*/



void Statement (uint lvl){ // any single line. always ends with NL
	if (token == T_NL){
		Match(T_NL);
		if(block_lvl <= lvl); // empty statement
		
		else { // subordinate block
			lvl=block_lvl;
			fprintf(outfile, "\t; START block level %u\n", lvl);
			do {
				Statement(lvl);
			} while (token != T_EOF && block_lvl == lvl);
			fprintf(outfile, "\t; END block level %u\n", lvl);
		}
	}
	else {
		switch (token){
		case T_LBL:   Label(   ); break;
		case T_JMP:   Jump (   ); break;
		case T_IF:    If   (lvl); break;
		case T_WHILE: While(lvl); break;
		default:
			Assignment_statement();
		}
	}
}


