/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


/******************************************************************************/
//                           CONTROL STATEMENTS
/******************************************************************************/


void Label(void){
	Match(T_LBL);
	emit_iop(add_name( get_name() ), I_NOP, NO_NAME, NULL, NULL, NULL);
	Match(T_NL);
}

void Jump(void){
	Match(T_JMP);
	emit_iop(NO_NAME, I_JMP, add_name(get_name()), NULL, NULL, NULL);
	Match(T_NL);
}

void If(uint lvl){
	sym_pt condition;
	name_dx if_label, else_label;
	
	Match(T_IF);
	emit_cmnt("start of IF statement");
	if_label = new_label();
	
	condition = Boolean();
	if(condition->type == st_lit_int){}
	emit_iop(NO_NAME, I_JZ, if_label, NULL, condition, NULL);
	
	Statement(lvl);
	
	if(token == T_ELSE){
		Match(T_ELSE);
		emit_cmnt("start of ELSE statement");
		else_label = new_label();
		emit_iop(NO_NAME, I_JMP, else_label, NULL, NULL, NULL);
		emit_iop(if_label, I_NOP, NO_NAME, NULL, NULL, NULL);
		
		Statement(lvl);
		
		emit_iop(else_label, I_NOP, NO_NAME, NULL, NULL, NULL);
		emit_cmnt("End of ELSE");
	}
	else emit_iop(if_label, I_NOP, NO_NAME, NULL, NULL, NULL);
	
	emit_cmnt("End of IF Statement\n");
}

void While(uint lvl){
	name_dx repeat_label, skip_label;
	sym_pt result;
	
	Match(T_WHILE);
	emit_cmnt("Start of WHILE loop");
	repeat_label = new_label();
	skip_label   = new_label();
	
	emit_iop(repeat_label, I_NOP, NO_NAME, NULL, NULL, NULL);
	
	result = Boolean();
	
	emit_iop(NO_NAME, I_JZ, skip_label, NULL, result, NULL);
	
	Statement(lvl);
	
	emit_iop(NO_NAME, I_JMP, repeat_label, NULL, NULL, NULL);
	emit_iop(skip_label, I_NOP, NO_NAME, NULL, NULL, NULL);
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
			//fprintf(outfile, "\t# START block level %u\n", lvl);
			do {
				Statement(lvl);
			} while (token != T_EOF && block_lvl == lvl);
			if(block_lvl > lvl) parse_error("Unexpected nested block");
			//fprintf(outfile, "\t# END block level %u\n", lvl);
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

