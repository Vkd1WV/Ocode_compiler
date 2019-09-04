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


static void Call_sub(void){
	
	
	// TODO: Setup Parameters
	
	Scope_Stack::emit_op(I_CALL, NULL, NULL, NULL);
}


/******************************************************************************/
//                           CONTROL STATEMENTS
/******************************************************************************/


static void Label(void){
	match_token(T_LBL);
	if(Scanner::token() != T_NAME) expected("An unbound name");
	Scope_Stack::emit_lbl( Program_data::add_string( Scanner::text() ), NULL );
	match_token(T_NAME);
	match_token(T_NL);
}

static void Jump(void){
	match_token(T_JMP);
	if(Scanner::token() != T_NAME) expected("An unbound name");
	Scope_Stack::emit_jump(Program_data::add_string( Scanner::text() ), NULL);
	match_token(T_NAME);
	match_token(T_NL);
}

static void Break(void){
	match_token(T_BRK);
	if(break_this == NO_NAME)
		parse_error("Break statement with no enclosing control loop");
	Scope_Stack::emit_jump(break_this, NULL);
	match_token(T_NL);
}

static void Continue(void){
	match_token(T_CNTN);
	if(continue_this == NO_NAME)
		parse_error("Continue statement with no enclosing control loop");
	Scope_Stack::emit_jump(continue_this, NULL);
	match_token(T_NL);
}

static void Return(void){
	sym_pt ret_val = NULL;
	
	match_token(T_RTRN);
	if(Scanner::token() != T_NL) ret_val = Boolean();
	else ret_val = Program_data::unq_sym(st_lit_int);
	match_token(T_NL);
	
	// TODO: check return type
	
	Scope_Stack::emit_op(I_RTRN, NULL, ret_val, NULL);
}


static void If(void){
	sym_pt condition;
	str_dx skip_label, else_label;
	
	msg_trace(logfile, "If(): start");
	
	match_token(T_IF);
	Scope_Stack::emit_cmnt("IF");
	
	else_label = Program_data::unq_label();
	
	condition = Boolean();
	if(condition->type == st_lit_int){
		if(!condition->value)
			Scope_Stack::emit_jump(else_label, NULL);
		// else fallthrough
		
		// remove unneeded symbol
		Program_data::remove_sym(condition->name);
	}
	else Scope_Stack::emit_jz(else_label, condition);
	
	Statement();
	
	if(Scanner::token() == T_ELSE){
		match_token(T_ELSE);
		
		skip_label = Program_data::unq_label();
		
		Scope_Stack::emit_jump(skip_label, NULL);
		Scope_Stack::emit_lbl(else_label, "ELSE lbl");
		
		Statement();
		
		Scope_Stack::emit_lbl(skip_label, "SKIP lbl");
	}
	else Scope_Stack::emit_lbl(else_label, "ELSE lbl");
	
	Scope_Stack::emit_cmnt("END IF");
	
	
	msg_trace(logfile, "If(): stop");
}


static void While(void){
	str_dx continue_label, break_label;
	sym_pt condition;
	
	// Save the previous break and continue labels
	continue_label = continue_this;
	break_label    = break_this;
	
	// create new labels for this loop
	continue_this = Program_data::unq_label();
	break_this    = Program_data::unq_label();
	
	
	match_token(T_WHILE);
	Scope_Stack::emit_cmnt("WHILE");
	Scope_Stack::emit_lbl(continue_this, "CONT lbl");
	
	condition = Boolean();
	if(condition->type == st_lit_int){
		if(!condition->value)
			Scope_Stack::emit_jump(break_this, NULL);
		
		// remove unneeded symbol
		Program_data::remove_sym(condition->name);
	}
	else Scope_Stack::emit_jz(break_this, condition);
	
	Statement();
	
	Scope_Stack::emit_jump(continue_this, NULL);
	Scope_Stack::emit_lbl(break_this, "BRK lbl");
	Scope_Stack::emit_cmnt("END WHILE");
	
	
	// restore previous break and continue labels
	continue_this = continue_label;
	break_this    = break_label;
}


static void Do(void){
	str_dx continue_label, break_label, true_label;
	sym_pt condition;
	
	msg_trace(logfile, "Do(): start");
	
	// Save the previous break and continue labels
	continue_label = continue_this;
	break_label    = break_this;
	
	// create new labels for this loop
	continue_this = Program_data::unq_label();
	break_this    = Program_data::unq_label();
	true_label    = Program_data::unq_label();
	
	
	match_token(T_DO);
	Scope_Stack::emit_cmnt("DO");
	Scope_Stack::emit_lbl(true_label, "TRUE lbl");
	
	Statement();
	
	// each continue retests the condition
	Scope_Stack::emit_lbl(continue_this, "CONT lbl");
	match_token(T_WHILE);
	condition = Boolean();
	match_token(T_NL);
	
	if(condition->type == st_lit_int){
		if(condition->value)
			Scope_Stack::emit_jump(true_label, NULL);
		
		// remove unneeded symbol
		Program_data::remove_sym(condition->name);
	}
	else Scope_Stack::emit_jump(true_label, condition);
	
	Scope_Stack::emit_lbl(break_this, "BRK lbl");
	Scope_Stack::emit_cmnt("END DO");
	
	// restore previous break and continue labels
	continue_this = continue_label;
	break_this    = break_label;
	
	msg_trace(logfile, "Do(): stop");
}


static void For(){
	match_token(T_FOR);
	
	// FIXME
	msg_print(NULL, V_ERROR, "Internal: For() is not completely implemented");
	
	Statement();
}

static void Case(void){}
static void Default(void){
	match_token(T_DFLT);
}

static void Switch(void){
	sym_pt condition;
	str_dx break_label;
	
	break_label = break_this;
	break_this = Program_data::unq_label();
	
	match_token(T_SWTCH);
	condition = Boolean();
	if(condition->type == st_lit_int){}
	
	// FIXME
	
	msg_print(NULL, V_ERROR, "Internal: Switch() is not completely implemented");
	
	while(Scanner::token() == T_CASE) Case();
	Default();
	
	Scope_Stack::emit_lbl(break_this, "BRK lbl");
	
	break_this = break_label;
}


static void Statement_block(void){
	match_token(T_IND);
	do{
		Statement();
	} while(Scanner::token() != T_OUTD);
	match_token(T_OUTD);
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


// lvl is the block level that the statement starts with
void Statement (void){ // any single line. always ends with NL
	sym_pt sym;
	
	#ifdef DBG_PARSE
	msg_trace(logfile, "Statement(): start");
	#endif
	
	if (Scanner::token() == T_NL){
		Scanner::next_token();
		if(Scanner::token() == T_IND) Statement_block();
		else return;
		
/*		sprintf(err_array, "Statement(): lvl: %u block_lvl: %u", lvl, block_lvl);*/
/*		debug_msg(err_array);*/
/*		if(block_lvl <= lvl); // empty statement*/
/*		// Empty statements like this may occur as subordinates of control*/
/*		// statements.*/
/*		*/
/*		else { // subordinate block*/
/*			lvl=block_lvl;*/
/*			//fprintf(outfile, "\t# START block level %u\n", lvl);*/
/*			do {*/
/*				Statement(lvl);*/
/*			} while (Scanner::token() != T_EOF && block_lvl == lvl);*/
/*			if(block_lvl > lvl) parse_error("Unexpected nested block");*/
/*			//fprintf(outfile, "\t# END block level %u\n", lvl);*/
//		}
	}
	else
		switch (Scanner::token()){
		// Declarations
		case T_8     :
		case T_16    :
		case T_32    :
		case T_64    :
		case T_WORD  :
		case T_MAX   :
		case T_PTR   :
		case T_N_TYPE: Decl_Storage (); break;
		//case T_SUB   : Decl_Sub     (); break;
		case T_FUN   : Decl_Fun     (); break;
		//case T_TYPE  : Decl_Type    (); break;
		//case T_OPR   : Decl_Operator(); break;
		//case T_NAME  : Decl_Implicit(); break;
		
		// Control Statements
		case T_LBL  : Label   (); break;
		case T_JMP  : Jump    (); break;
		case T_BRK  : Break   (); break;
		case T_CNTN : Continue(); break;
		case T_RTRN : Return  (); break;
		case T_IF   : If      (); break;
		case T_WHILE: While   (); break;
		case T_DO   : Do      (); break;
		case T_FOR  : For     (); break;
		case T_SWTCH: Switch  (); break;
		case T_N_SUB:Call_sub (); break;
		default:
			sym = Boolean();
			if(sym->type == st_lit_int){
				parse_warn("Statement with no effect");
				Program_data::remove_sym(sym->name);
			}
			
			match_token(T_NL);
		}
	
	#ifdef DBG_PARSE
	msg_trace(logfile, "Statement(): stop");
	#endif
}

