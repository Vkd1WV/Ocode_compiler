/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

// generates the fully qualified name
static void set_name(sym_pt sym, char * short_name){
	char * prefix, *buffer;
	size_t size;
	
	prefix = scope_prefix();
	
	// make a buffer to combine them
	size = strlen(prefix) + strlen(short_name) +1;
	buffer = malloc(size);
	
	strcpy(buffer, prefix);
	strcat(buffer, short_name);
	
	sym->name = add_name(buffer);
	sym->short_name = sym->name + strlen(prefix);
	
	free(buffer);
}

/*static DS     New_mem_list (void){}*/
/*static sym_pt First_member (DS mem_lst){}*/
/*static sym_pt Next_member  (DS mem_lst){}*/
/*static sym_pt Add_member   (sym_pt member){}*/
/*static void   Merge_mem_lst(DS lst_a, DS lst_b){}*/


/******************************************************************************/
//                           BASIC DECLARATIONS
/******************************************************************************/


static void Qualifier_list   (sym_pt templt){
	if (token == T_STATIC){
		templt->stat = true;
		get_token();
	}
	else if (token == T_CONST){
		templt->constant = true;
		get_token();
	}
	
	// storage class specifier
	// auto static extern register
}

static void Initializer_list (sym_pt templt){
	sym_pt new_symbol, initializer;
	
	while (true) {
		set_name(templt, get_name());
		
		if(!( new_symbol = (sym_pt)DS_insert(symbols, templt) ))
			parse_error("Duplicate symbol name");
		
		debug_sym("Initializer_list():Adding symbol", new_symbol);
		
		if(!new_symbol->stat){} // TODO: we have to do something special here
		
		if (token == T_EQ){ // Initialized value
			get_token();
			
			if(new_symbol->type != st_int && new_symbol->type != st_ref)
				parse_error("Invalid target for initialization");
			
			
			initializer=Boolean();
			if (!initializer->set) err_msg("Using an uninitialized value");
			if (initializer->type != st_lit_int)
				parse_error("Initializers must be constant expressions");
			
			new_symbol->value = initializer->value;
			new_symbol->set   = true;
			new_symbol->init  = true;
			
			// remove the initializer
			if( DS_find(symbols, dx_to_name(initializer->name)) )
				DS_remove(symbols);
		}
		else if (new_symbol->constant)
			parse_error("No initialization for constant");
		
		if      (token == T_LIST) { Match(T_LIST); continue; }
		else if (token == T_NL  ) { Match(T_NL)  ; break   ; }
		else    expected("a comma or newline");
	};
	
	/* Declarator list
	 * comma separated list of declarators
	*/
}

// Declare a Pointer
static void Decl_Pointer (sym_pt templt){
	sym_pt target;
	
	templt->type = st_ref;
	
	target= new_var(st_undef);
	target->set = true; // assume it's initialized
	
	templt->dref = target;
	
	Match(T_PTR);
	
	Qualifier_list(templt);
	
	Match_string("to");
	
	Type_specifier(target);
}

// Declare a Word
static void Decl_Word(sym_pt templt){
	templt->type = st_int;
	
	switch (token){
		case T_8:    templt->size=w_byte ; break;
		case T_16:   templt->size=w_byte2; break;
		case T_32:   templt->size=w_byte4; break;
		case T_64:   templt->size=w_byte8; break;
		case T_WORD: templt->size=w_word ; break;
		case T_MAX:  templt->size=w_max  ; break;
		default: crit_error("Internal compiler error at Decl_word()");
	}
	get_token();
	
	Qualifier_list(templt);
}

static void Decl_Custom (sym_pt templt){
	sym_pt type_def;
	
/*	type_def = (sym_pt)Bind(get_name());*/
/*	if(!type_def)*/
/*		crit_error("Internal: Decl_Custom(): could not bind type definition");*/
	
/*	templt->type = st_cust;*/
/*	templt->members = type_def->members;*/
	
	//TODO: alias?
	
	Qualifier_list(templt);
}

void Type_specifier(sym_pt templt_pt){
	
	switch(token){
		case T_8:
		case T_16:
		case T_32:
		case T_64:
		case T_WORD:
		case T_MAX:  Decl_Word   (templt_pt); break;
		case T_PTR:  Decl_Pointer(templt_pt); break;
		default:     Decl_Custom (templt_pt); break;
	}
	
	// type specifier
	/* int
	 * char
	 * float
	 * double
	 * struct identifier
	*/
}

static void Decl_Symbol_list(void){
	struct sym templt;
	
	// Initialize the template
	memset((void*) &templt, 0, sizeof(struct sym));
	
	Type_specifier(&templt);
	
	Initializer_list(&templt);
	
} // end Decl_Symbol


/******************************************************************************/
//                         PROCEDURE DECLARATIONS
/******************************************************************************/


// Parameter lists for functions and subroutines
static void Parameter_list(sym_pt procedure){
	struct sym sym[1];
	sym[1] = sym[1];
	procedure = procedure;
	/*	
	 *
	 */
	
	while(token != T_CBRK && token != T_NL) get_token();
	
/*	while (true){*/
/*		switch(token){*/
/*		case T_IN:*/
/*		case T_OUT:*/
/*		case T_BI:*/
/*		default:*/
/*		}*/
/*	}*/
}

// Declare an Operator
static void Decl_Operator(uint lvl){
	struct sym oper;
	sym_pt new_op = &oper;
	bool assem;
	
	new_op->type = st_fun;
	
	get_token();
	if(token == T_ASM) assem = true;
	
	switch(token){
	case T_OPAR :
	case T_CPAR :
	case T_OBRC :
	case T_CBRC :
	case T_OBRK :
	case T_CBRK :
	case T_DEC  :
	case T_INC  :
	case T_REF  :
	case T_DREF :
	case T_NOT  :
	case T_INV  :
	case T_PLUS :
	case T_MINUS:
	case T_BAND :
	case T_BOR  :
	case T_BXOR :
	case T_MUL  :
	case T_MOD  :
	case T_DIV  :
	case T_EXP  :
	case T_LSHFT:
	case T_RSHFT:
	case T_EQ   :
	case T_NEQ  :
	case T_LT   :
	case T_GT   :
	case T_LTE  :
	case T_GTE  :
	case T_AND  :
	case T_OR   :
	case T_ASS  :
	case T_LSH_A:
	case T_RSH_A:
	case T_ADD_A:
	case T_SUB_A:
	case T_MUL_A:
	case T_DIV_A:
	case T_MOD_A:
	case T_AND_A:
	case T_OR_A :
	case T_XOR_A:
	case T_LIST : break;
	
	// Everything else is an error
	case T_EOF :
	case T_NL  :
	case T_NUM :
	case T_NAME:
	case T_STR :
	case T_CHAR:
		parse_error("Decl_Operator(): Cannot use a Primary type for an operator declaration");
	case T_LBL  :
	case T_JMP  :
	case T_IF   :
	case T_ELSE :
	case T_SWTCH:
	case T_CASE :
	case T_DFLT :
	case T_WHILE:
	case T_DO   :
	case T_BRK  :
	case T_CNTN :
	case T_TRY  :
	case T_THRW :
	case T_CTCH :
	case T_FOR  :
	case T_RTRN :
	case T_OPR :
	case T_SUB :
	case T_FUN :
	case T_TYPE:
	case T_8   :
	case T_16  :
	case T_32  :
	case T_64  :
	case T_WORD:
	case T_MAX :
	case T_PTR:
	case T_CONST :
	case T_STATIC:
	case T_ASM:
	case T_IN:
	case T_OUT:
	case T_BI:
		parse_error("Decl_Operator(): Cannot use a keyword for an operator declaration");
	case NUM_TOKENS:
	default: parse_error("Internal: Decl_Operator(): unknown operator");
	}
	
	//Name
	set_name(new_op, token_dex[token]);
	get_token();
	if(!( new_op = DS_insert(symbols, new_op) )){
		sprintf(
			err_array,
			"Redeclaration of Operator %s",
			dx_to_name(new_op->name)
		);
		parse_error(err_array);
	}
	
	push_scope(new_op);
	
	// Parameters
	
	Match(T_NL);
	
	if(assem){
		
	}
	else{
		Decl_list(lvl+1);
		Statement(lvl+1);
	}
	
	pop_scope();
	
	// End statement
	Match_string("end");
	Match_string(dx_to_name(new_op->name)); // FIXME
	Match(T_NL);
}

// Declare a Subroutine
static void Decl_Sub(uint lvl){
	struct sym sub;
	sym_pt new_sub = &sub;
	bool assem;
	
	new_sub->type  = st_sub;
	
	get_token();
	if(token == T_ASM) assem = true;
	
	// Name
	set_name(new_sub, get_name());
	if(!( new_sub = DS_insert(symbols, new_sub) )){
		sprintf(
			err_array,
			"Redeclaration of subroutine %s",
			dx_to_name(new_sub->name)
		);
		parse_error(err_array);
	}
	
	push_scope(new_sub);
	
	// Parameter Declarations
	Parameter_list(new_sub);
	Match(T_NL);
	
	if(assem){
		
	}
	else{
		Decl_list(lvl+1);
		Statement(lvl+1);
	}
	
	pop_scope();
	
	// End statement
	Match_string("end");
	Match_string(dx_to_name(new_sub->name));
	Match(T_NL);
}


// Declare a Function
static void Decl_Fun (uint lvl){
	struct sym fun;
	sym_pt new_fun = &fun;
	bool assem;
	
	new_fun->type  = st_fun;
	
	Match(T_FUN);
	if(token == T_ASM) assem = true;
	
	// Name
	set_name(new_fun, get_name());
	if(!( new_fun = DS_insert(symbols, new_fun) )){
		sprintf(
			err_array,
			"Redeclaration of function %s",
			dx_to_name(new_fun->name)
		);
		parse_error(err_array);
	}
	
	push_scope(new_fun);
	
	Match(T_OBRK);
	Parameter_list(new_fun);
	Match(T_CBRK);
	
	// Return type
	Match_string("returns");
	
	Match(T_NL);
	
	if(assem){
		
	}
	else{
		Decl_list(lvl+1);
		Statement(lvl+1);
	}
	
	pop_scope();
	
	// End statement
	Match_string("end");
	Match_string(dx_to_name(new_fun->name));
	Match(T_NL);
}


/******************************************************************************/
//                           TYPE DECLARATIONS
/******************************************************************************/


static void Decl_block(uint lvl){
	if (token == T_NL){
		get_token();
		if(block_lvl <= lvl) expected("A type declaration block");
		
		else { // subordinate block
			lvl=block_lvl;
			Decl_list(lvl);
			if(block_lvl > lvl) parse_error("Unexpected nested block");
		}
	}
	else Declaration(lvl);
	
	
	sprintf(err_array, "Decl_block(): stop with token: %d", token);
	debug_msg(err_array);
}

// Define a Datatype
static void Decl_Type (uint lvl){
	struct sym t;
	sym_pt new_type = &t;
	
	Match(T_TYPE);
	set_name(new_type, get_name());
	
	if(!( new_type = DS_insert(symbols, new_type) ))
		parse_error("Duplicate type definition");
	
	new_type->type = st_type_def;
	
	push_scope(new_type);
	
	Decl_block(lvl);
	
	pop_scope();
}


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


// returns whether a declaration was found
bool Declaration(uint lvl){
	sym_pt sym;

	switch (token){
	case T_TYPE: Decl_Type    (lvl); return true;
	
	case T_SUB : Decl_Sub     (lvl); return true;
	case T_FUN : Decl_Fun     (lvl); return true;
	case T_OPR : Decl_Operator(lvl); return true;
	
	case T_8   :
	case T_16  :
	case T_32  :
	case T_64  :
	case T_WORD:
	case T_MAX :
	case T_PTR : 
	case T_N_TYPE: Decl_Symbol_list (   ); return true;
/*		if(!( sym = (sym_pt) Bind(yytext) ))*/
/*			parse_error("Undeclared symbol");*/
/*		if(sym->type == st_type_def){ // defined type*/
/*			Decl_Symbol_list();*/
/*			return true;*/
/*		}*/
		// fall through
	
	default: return false;
	}
}

void Decl_list(uint lvl){
	debug_msg("Decl_list(): start");
	
	if(block_lvl != lvl) parse_error("Decl_list(): No Body");
	
	while(block_lvl == lvl){
		while(token == T_NL) get_token();
		if(!Declaration(lvl)) break;
	}
	
	sprintf(
		err_array,
		"Decl_list(): stop with token: %s on line %d",
		token_dex[token],
		yylineno
	);
	debug_msg(err_array);
}


