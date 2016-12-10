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


void Call_sub(sym_pt subroutine){}


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
	name_dx repeat_label, skip_label;
	sym_pt condition;
	
	Match(T_WHILE);
	emit_iop(NO_NAME, I_NOP, add_name("WHILE"), NULL, NULL, NULL);
	
	repeat_label = new_label();
	skip_label   = new_label();
	
	emit_iop(repeat_label, I_NOP, add_name("REPT lbl"), NULL, NULL, NULL);
	
	condition = Boolean();
	if(condition->type == st_lit_int){}
	
	emit_iop(NO_NAME, I_JZ, skip_label, NULL, condition, NULL);
	
	Statement(lvl);
	
	emit_iop(NO_NAME, I_JMP, repeat_label, NULL, NULL, NULL);
	emit_iop(skip_label, I_NOP, add_name("SKIP lbl"), NULL, NULL, NULL);
	emit_iop(NO_NAME, I_NOP, add_name("END WHILE"), NULL, NULL, NULL);
}

void Do(uint lvl){}

void For(uint lvl){}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


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
		case T_PTR : Decl_Symbol  (   ); break;
		case T_SUB : Decl_Sub     (lvl); break;
		case T_FUN : Decl_Fun     (lvl); break;
		case T_TYPE: Decl_Type    (lvl); break;
		case T_OPR : Decl_Operator(lvl); break;
		
		// Control Statements
		case T_LBL:   Label    (   ); break;
		case T_JMP:   Jump     (   ); break;
		case T_IF:    If       (lvl); break;
		case T_WHILE: While    (lvl); break;
		case T_DO:    Do       (lvl); break;
		case T_FOR:   For      (lvl); break;
		
		case T_NAME: // call sub or declare var of type_def maybe
			// we don't want to get the next token because we may fall through
			if(!( sym = (sym_pt) DS_find(symbols, yytext) ))
				parse_error("Undeclared symbol");
			if(sym->type == st_sub){
				get_name();
				Call_sub(sym);
				break;
			}
			if(sym->type == st_type_def){
				get_name();
				Decl_Custom(sym);
				break;
			}
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

