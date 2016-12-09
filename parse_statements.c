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
	emit_iop(
		NO_NAME,
		I_CMNT, add_name("start of IF statement"),
		NULL, NULL, NULL
	);
	if_label = new_label();
	
	condition = Boolean();
	if(condition->type == st_lit_int){}
	emit_iop(NO_NAME, I_JZ, if_label, NULL, condition, NULL);
	
	Statement(lvl);
	
	if(token == T_ELSE){
		Match(T_ELSE);
		emit_iop(
			NO_NAME,
			I_CMNT, add_name("start of ELSE statement"),
			NULL, NULL, NULL
		);
		else_label = new_label();
		emit_iop(NO_NAME, I_JMP, else_label, NULL, NULL, NULL);
		emit_iop(if_label, I_NOP, NO_NAME, NULL, NULL, NULL);
		
		Statement(lvl);
		
		emit_iop(else_label, I_NOP, NO_NAME, NULL, NULL, NULL);
		emit_iop(
			NO_NAME,
			I_CMNT, add_name("End of ELSE"),
			NULL, NULL, NULL
		);
	}
	else emit_iop(if_label, I_NOP, NO_NAME, NULL, NULL, NULL);
	
	emit_iop(
		NO_NAME,
		I_CMNT, add_name("End of IF Statement\n"),
		NULL, NULL, NULL
	);
}

void While(uint lvl){
	name_dx repeat_label, skip_label;
	sym_pt result;
	
	Match(T_WHILE);
	emit_iop(
		NO_NAME,
		I_CMNT, add_name("Start of WHILE loop"),
		NULL, NULL, NULL
	);
	repeat_label = new_label();
	skip_label   = new_label();
	
	emit_iop(repeat_label, I_NOP, NO_NAME, NULL, NULL, NULL);
	
	result = Boolean();
	
	emit_iop(NO_NAME, I_JZ, skip_label, NULL, result, NULL);
	
	Statement(lvl);
	
	emit_iop(NO_NAME, I_JMP, repeat_label, NULL, NULL, NULL);
	emit_iop(skip_label, I_NOP, NO_NAME, NULL, NULL, NULL);
	emit_iop(
		NO_NAME,
		I_CMNT, add_name("End of WHILE loop\n"),
		NULL, NULL, NULL
	);
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void Statement (uint lvl){ // any single line. always ends with NL
	sym_pt result;
	
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
			result = Boolean();
			if(result->type == st_lit_int){
				parse_warn("Statement with no effect");
				if(DS_find(symbols, dx_to_name(result->name)))
					DS_remove(symbols);
			}
			
			Match(T_NL);
	}
}

