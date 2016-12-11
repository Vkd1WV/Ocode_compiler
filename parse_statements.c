/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


/******************************************************************************/
//                           SUBROUTINE CALL
/******************************************************************************/


void Call_sub(sym_pt subroutine){
	
	
	// Setup Parameters
	
	emit_iop(NO_NAME, I_CALL, subroutine->name, NULL, NULL, NULL);
}


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

void Break(void){
	Match(T_BRK);
	if(break_this == NO_NAME)
		parse_error("Break statement with no enclosing control loop");
	
	emit_iop(NO_NAME, I_JMP, break_this, NULL, NULL, NULL);
	Match(T_NL);
}

void Continue(void){
	Match(T_CNTN);
	if(continue_this == NO_NAME)
		parse_error("Continue statement with no enclosing control loop");
	
	emit_iop(NO_NAME, I_JMP, continue_this, NULL, NULL, NULL);
	Match(T_NL);
}

void Return(void){
	Match(T_RTRN);
	
	// TODO: check return type and set it
	
	emit_iop(NO_NAME, I_RTRN, NO_NAME, NULL, NULL, NULL);
	Match(T_NL);
}

void If(uint lvl){
	sym_pt condition;
	name_dx skip_label, else_label;
	
	Match(T_IF);
	emit_iop(NO_NAME, I_NOP, add_name("IF"), NULL, NULL, NULL);
	
	else_label = new_label();
	
	condition = Boolean();
	if(condition->type == st_lit_int){}
	emit_iop(NO_NAME, I_JZ, else_label, NULL, condition, NULL);
	
	Statement(lvl);
	
	if(token == T_ELSE){
		Match(T_ELSE);
		
		skip_label = new_label();
		
		emit_iop(NO_NAME, I_JMP, skip_label, NULL, NULL, NULL);
		emit_iop(else_label, I_NOP, add_name("ELSE lbl"), NULL, NULL, NULL);
		
		Statement(lvl);
		
		emit_iop(skip_label, I_NOP, add_name("SKIP lbl"), NULL, NULL, NULL);
	}
	else emit_iop(else_label, I_NOP, add_name("ELSE lbl"), NULL, NULL, NULL);
	
	emit_iop(NO_NAME, I_NOP, add_name("END IF"), NULL, NULL, NULL);
}

void While(uint lvl){
	name_dx continue_label, break_label;
	sym_pt condition;
	
	// Save the previous break and continue labels
	continue_label = continue_this;
	break_label    = break_this;
	
	// create new labels for this loop
	continue_this = new_label();
	break_this    = new_label();
	
	
	Match(T_WHILE);
	emit_iop(NO_NAME, I_NOP, add_name("WHILE"), NULL, NULL, NULL);
	emit_iop(continue_this, I_NOP, add_name("CONT lbl"), NULL, NULL, NULL);
	
	condition = Boolean();
	if(condition->type == st_lit_int){}
	
	emit_iop(NO_NAME, I_JZ, break_this, NULL, condition, NULL);
	
	Statement(lvl);
	
	emit_iop(NO_NAME, I_JMP, continue_this, NULL, NULL, NULL);
	emit_iop(break_this, I_NOP, add_name("BRK lbl"), NULL, NULL, NULL);
	emit_iop(NO_NAME, I_NOP, add_name("END WHILE"), NULL, NULL, NULL);
	
	
	// restore previous break and continue labels
	continue_this = continue_label;
	break_this    = break_label;
}

void Do(uint lvl){
	name_dx continue_label, break_label;
	sym_pt condition;
	
	// Save the previous break and continue labels
	continue_label = continue_this;
	break_label    = break_this;
	
	// create new labels for this loop
	continue_this = new_label();
	break_this    = new_label();
	
	
	Match(T_DO);
	emit_iop(continue_this, I_NOP, add_name("CONT lbl"), NULL, NULL, NULL);
	
	Statement(lvl);
	
	Match(T_WHILE);
	condition = Boolean();
	if(condition->type == st_lit_int){}
	
	emit_iop(NO_NAME, I_JMP, continue_this, NULL, condition, NULL);
	emit_iop(break_this, I_NOP, add_name("BRK lbl"), NULL, NULL, NULL);
	
	// restore previous break and continue labels
	continue_this = continue_label;
	break_this    = break_label;
}

void For(uint lvl){
	Match(T_FOR);
	
	// FIXME
	err_msg("Internal: For() is not completely implemented");
	
	Statement(lvl);
}

void Switch(uint lvl){
	sym_pt condition;
	name_dx break_label;
	
	break_label = break_this;
	break_this = new_label();
	
	Match(T_SWTCH);
	condition = Boolean();
	if(condition->type == st_lit_int){}
	
	// FIXME
	
	err_msg("Internal: Switch() is not completely implemented");
	
	while(token == T_CASE) Statement(lvl);
	Match(T_DFLT);
	Statement(lvl);
	
	emit_iop(break_this, I_NOP, NO_NAME, NULL, NULL, NULL);
	
	break_this = break_label;
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


// lvl is the block level that the statement starts with
void Statement (uint lvl){ // any single line. always ends with NL
	sym_pt sym;
	
	if (token == T_NL){
		get_token();
		if(block_lvl <= lvl); // empty statement
		// Empty statements like this may occur as subordinates of control
		// statements.
		
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
	
	else switch (token){
		
		// Declarations
		case T_8   :
		case T_16  :
		case T_32  :
		case T_64  :
		case T_WORD:
		case T_MAX :
		case T_PTR :
		case T_SUB :
		case T_FUN :
		case T_TYPE:
		case T_OPR :
			sprintf(err_array, "token is: %d", token);
			err_msg(err_array);
			parse_error("Declaration found in statement section");
		
		// Control Statements
		case T_LBL  : Label    (   ); break;
		case T_JMP  : Jump     (   ); break;
		case T_IF   : If       (lvl); break;
		case T_WHILE: While    (lvl); break;
		case T_DO   : Do       (lvl); break;
		case T_FOR  : For      (lvl); break;
		case T_SWTCH: Switch   (lvl); break;
		case T_BRK  : Break    (   ); break;
		case T_CNTN : Continue (   ); break;
		case T_RTRN : Return   (   ); break;
		
		case T_NAME: // call sub or declare var of type_def maybe
			// we don't want to get the next token because we may fall through
			if(!( sym = (sym_pt) DS_find(symbols, yytext) ))
				parse_error("Undeclared symbol");
			if(sym->type == st_sub){
				get_name();
				Call_sub(sym);
				break;
			}
			if(sym->type == st_type_def)
				parse_error("Declaration found in statement section");
			
			// else fall through
		default:
			sym = Boolean();
			if(sym->type == st_lit_int){
				parse_warn("Statement with no effect");
				if(DS_find(symbols, dx_to_name(sym->name)))
					DS_remove(symbols);
			}
			
			Match(T_NL);
	}
}

