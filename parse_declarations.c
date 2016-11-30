/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "compiler.h"





/******************************************************************************/
//                             DECLARATION LISTS
/******************************************************************************/


void Qualifier_list   (sym_pt templt){
	if (token == T_STATIC){
		templt->stat = true;
		get_token();
	}
	else if (token == T_CONST){
		templt->constant = true;
		get_token();
	}
/*	else if (token == T_ASM){*/
/*		if (!( templt->type == function || templt->type == subroutine ))*/
/*			parse_error("Invalid use of assembler qualifier in declaration");*/
/*		templt->assembler = true;*/
/*		get_token();*/
/*	}*/
}

void Initializer_list (sym_pt templt){
	sym_pt new_symbol, initializer;
	
	while (true) {
		templt->name = add_name(get_name());
		
		if(!( new_symbol = (sym_pt)DS_insert(symbols, templt) ))
			parse_error("Duplicate symbol name");
		
		debug_sym("Initializer_list(): Adding symbol", new_symbol);
		
		if (token == T_ASS){ // Initialized value
			get_token();
			
			if(new_symbol->type != st_int && new_symbol->type != st_ref)
				parse_error("Invalid target for initialization");
			new_symbol->init = true;
			
			initializer=Boolean();
			//if (!initializer->init) error("Using an uninitialized value");
			
			emit_iop(NO_NAME, I_ASS, 0, new_symbol, initializer, NULL);
		}
		else if (new_symbol->constant)
			parse_error("No initialization for constant");
		
		if      (token == T_LIST) { Match(T_LIST); continue; }
		else if (token == T_NL  ) { Match(T_NL)  ; break   ; }
		else    expected("a comma or newline");
	};
}

// Parameter lists for functions and subroutines
void Parameter_list   (sym_pt templt){
	templt = templt;
}


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


// Define a Datatype
void Decl_Type (sym_pt templt){
	templt = templt;
}

// Declare a Subroutine
void Decl_Sub(sym_pt new_sub){
	new_sub->type  = st_sub;
	//new_sub->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_sub->assembler = true;
	
	// Name
	new_sub->name = add_name(get_name());
	if(! DS_insert(symbols, new_sub) )
		parse_error("Duplicate symbol name");
	
	// Parameter & Return Declarations
	
	Statement(1);
	
	// End statement
	Match(T_END);
	Match_name(new_sub->name);
	Match(T_NL);
}

// Declare a Function
void Decl_Fun (sym_pt new_fun){
	new_fun->type  = st_fun;
	//new_fun->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_fun->assembler = true;
	
	// Name
	new_fun->name = add_name(get_name());
	if(! DS_insert(symbols, new_fun) )
		parse_error("Duplicate symbol name");
	
	// Parameter & Return Declarations
	
	Statement(1);
	
	// End statement
	Match(T_END);
	Match_name(new_fun->name);
	Match(T_NL);
}

// Declare a Pointer
void Decl_Pointer (sym_pt templt){
	sym_pt target;
	
	templt->type = st_ref;
	
	target= (sym_pt) calloc(1, sizeof(struct sym));
	if (!target) crit_error("Out of memory");
	
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
		case T_TYPE: Decl_Type   (templt_pt); break;
		case T_SUB:  Decl_Sub    (templt_pt); break;
		case T_FUN:  Decl_Fun    (templt_pt); break;
		default:     Decl_Word   (templt_pt); break;
	}
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


// Declare an Operator
void Decl_Operator(){}

void Decl_Symbol  (void){
	struct sym templt;
	
	//puts("Decl_Symbol");
	
	// Initialize the template
	memset((void*) &templt, 0, sizeof(struct sym));
	
	Type_specifier(&templt);
	
	Initializer_list(&templt);
	
} // end Decl_Symbol


