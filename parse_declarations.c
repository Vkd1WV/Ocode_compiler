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

/******************************************************************************/
//                             DECLARATION LISTS
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
		templt->name = add_name(get_name());
		
		if(!( new_symbol = (sym_pt)DS_insert(symbols, templt) ))
			parse_error("Duplicate symbol name");
		
		debug_sym("Initializer_list():Adding symbol", new_symbol);
		
		if(!new_symbol->stat){} // TODO: we have to do something special here
		
		if (token == T_ASS){ // Initialized value
			get_token();
			
			if(new_symbol->type != st_int && new_symbol->type != st_ref)
				parse_error("Invalid target for initialization");
			new_symbol->init = true;
			
			initializer=Boolean();
			//if (!initializer->init) error("Using an uninitialized value");
			
			emit_iop(NO_NAME, I_ASS, NO_NAME, new_symbol, initializer, NULL);
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

// Parameter lists for functions and subroutines
static void Parameter_list   (sym_pt templt){
	templt = templt;
}


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


// Declare a Pointer
void Decl_Pointer (sym_pt templt){
	sym_pt target;
	
	templt->type = st_ref;
	
	target= (sym_pt) calloc(1, sizeof(struct sym));
	if (!target) crit_error("Out of memory");
	
	target->name = NO_NAME;
	target->init = true; // assume it's initialized
	
	templt->dref = target;
	
	Match(T_PTR);
	
	Qualifier_list(templt);
	
	Match(T_TO);
	
	Type_specifier(target);
}

// Declare a Word
void Decl_Word(sym_pt templt){
	templt->type = st_int;
	
	//puts("Decl_Word");
	
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

void Type_specifier(sym_pt templt_pt){
	
	switch(token){
		case T_PTR:  Decl_Pointer(templt_pt); break;
		default:     Decl_Word   (templt_pt); break;
	}
	
	// type specifier
	/* int
	 * char
	 * float
	 * double
	 * struct identifier
	*/
}



// Define a Datatype
void Decl_Type (uint lvl){
	struct sym t;
	sym_pt new_type = &t;
	
	Match(T_TYPE);
	new_type->name = add_name(get_name());
	
	if(!( new_type = DS_insert(symbols, new_type) ))
		parse_error("Duplicate type definition");
	
	new_type->type = st_type_def;
	
	push_scope(new_type);
	
	Decl_list(lvl);
	
	pop_scope();
}

// Declare an instance of a defined type
void Decl_Custom(sym_pt templt){}

// Declare an Operator
void Decl_Operator(uint lvl){}


// Declare a Subroutine
void Decl_Sub(uint lvl){
	struct sym sub;
	sym_pt new_sub = &sub;
	
	new_sub->type  = st_sub;
	
	get_token();
	if(token == T_ASM) new_sub->assembler = true;
	
	// Name
	new_sub->name = add_name(get_name());
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
	
	// Get declarations
	Decl_list(lvl);
	
	Statement(lvl);
	
	pop_scope();
	
	// End statement
	Match(T_END);
	Match_name(new_sub->name);
	Match(T_NL);
}


// Declare a Function
void Decl_Fun (uint lvl){
	struct sym fun;
	sym_pt new_fun = &fun;
	
	new_fun->type  = st_fun;
	
	get_token();
	if(token == T_ASM) new_fun->assembler = true;
	
	// Name
	new_fun->name = add_name(get_name());
	if(!( new_fun = DS_insert(symbols, new_fun) )){
		sprintf(
			err_array,
			"Redeclaration of function %s",
			dx_to_name(new_fun->name)
		);
		parse_error(err_array);
	}
	
	push_scope(new_fun);
	
	// Return type
	
	Match(T_OBRK);
	Parameter_list(new_fun);
	Match(T_CBRK);
	
	Decl_list(lvl);
	
	Statement(lvl);
	
	pop_scope();
	
	// End statement
	Match(T_END);
	Match_name(new_fun->name);
	Match(T_NL);
}


void Decl_Symbol  (void){
	struct sym templt;
	
	// Initialize the template
	memset((void*) &templt, 0, sizeof(struct sym));
	
	Type_specifier(&templt);
	
	Initializer_list(&templt);
	
} // end Decl_Symbol


void Decl_list(uint lvl){
	sym_pt sym;
	
	do{
		switch (token){
		case T_IF: continue; // must use constant expressions
		
		case T_8   :
		case T_16  :
		case T_32  :
		case T_64  :
		case T_WORD:
		case T_MAX :
		case T_PTR : Decl_Symbol  (   ); continue;
		case T_SUB : Decl_Sub     (lvl); continue;
		case T_FUN : Decl_Fun     (lvl); continue;
		case T_TYPE: Decl_Type    (lvl); continue;
		case T_OPR : Decl_Operator(lvl); continue;
		case T_NAME:
			if(!( sym = (sym_pt) Bind(yytext) ))
				parse_error("Undeclared symbol");
			if(sym->type == st_type_def){
				get_name();
				Decl_Custom(sym);
				continue;
			}
			// fall through
		default: break;
		}
	} while (false);
}



