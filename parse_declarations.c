

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Decl_Word    (sym_pt templt);
void Decl_Pointer (sym_entry* templt);
void Decl_Type    (sym_entry* templt);
void Decl_Sub     (sym_entry* new_sub);
void Decl_Fun     (sym_entry* new_fun);

void Type_specifier(sym_pt templt_pt);

void Qualifier_list   (sym_entry* templt);
void Initializer_list (sym_entry* templt);
void Parameter_list   (sym_entry* templt);


/******************************************************************************/
//                             DECLARATION LISTS
/******************************************************************************/


void Qualifier_list   (sym_entry* templt){
	if (token == T_STATIC){
		templt->stat = true;
		get_token();
	}
	else if (token == T_CONST){
		templt->constant = true;
		get_token();
	}
	else if (token == T_ASM){
		if (!( templt->type == function || templt->type == subroutine ))
			parse_error("Invalid use of assembler qualifier in declaration");
		templt->assembler = true;
		get_token();
	}
}

void Initializer_list (sym_entry* templt){
	      sym_entry * new_symbol;
	const sym_entry * initializer;
	
	while (true) {
		strncpy(templt->name, get_name(), NAME_MAX);
		if(templt->name[0] == '_' && templt->name[1] == '_')
			parse_error("Names begining with __ are reserved for internal compiler use");
		
		if( DS_sort(global_symbols, templt) )
			parse_error("Duplicate symbol name");
		new_symbol = DS_current(global_symbols);
		
		if (token == T_ASS){ // Initialized value
			get_token();
			
			if(new_symbol->type != data && new_symbol->type != pointer)
				parse_error("Invalid target for initialization");
			new_symbol->init = true;
			
			initializer=Boolean();
			//if (!initializer->init) error("Using an uninitialized value");
			
			emit_quad(I_ASS, new_symbol, initializer, NULL);
		}
		else if (new_symbol->constant)
			parse_error("No initialization for constant");
		
		if      (token == T_LIST) { Match(T_LIST); continue; }
		else if (token == T_NL  ) { Match(T_NL)  ; break   ; }
		else    expected("a comma or newline");
	};
}

// Parameter lists for functions and subroutines
void Parameter_list   (sym_entry* templt){
	templt = templt;
}


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


// Define a Datatype
void Decl_Type (sym_entry* templt){
	templt = templt;
}

// Declare a Subroutine
void Decl_Sub(sym_entry* new_sub){
	new_sub->type  = subroutine;
	//new_sub->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_sub->assembler = true;
	
	// Name
	strncpy(new_sub->name, get_name(), NAME_MAX);
	if( DS_sort(global_symbols, new_sub) )
		parse_error("Duplicate symbol name");
	
	// Parameter & Return Declarations
	
	Statement(1);
	
	// End statement
	Match(T_END);
	if (strcmp( new_sub->name, get_name() ))
		parse_error("Miss-matched end statement");
	Match(T_NL);
}

// Declare a Function
void Decl_Fun (sym_entry* new_fun){
	new_fun->type  = function;
	//new_fun->local = new_DS('l');
	
	get_token();
	if(token == T_ASM) new_fun->assembler = true;
	
	// Name
	strncpy(new_fun->name, get_name(), NAME_MAX);
	if( DS_sort(global_symbols, new_fun) )
		parse_error("Duplicate symbol name");
	
	// Parameter & Return Declarations
	
	Statement(1);
	
	// End statement
	Match(T_END);
	if (strcmp( new_fun->name, get_name() ))
		parse_error("Miss-matched end statement");
	Match(T_NL);
}

// Declare a Pointer
void Decl_Pointer (sym_pt templt){
	sym_pt target;
	
	templt->type = pointer;
	
	target=calloc(1, sizeof(sym_entry));
	if (!target) crit_error("Out of memory");
	
	templt->dref = target;
	
	Match(T_PTR);
	
	Qualifier_list(templt);
	
	Match(T_TO);
	
	Type_specifier(target);
}

// Declare a Word
void Decl_Word(sym_pt templt){
	templt->type = data;
	
	//puts("Decl_Word");
	
	switch (token){
		case T_8:    templt->size=byte   ; break;
		case T_16:   templt->size=byte2  ; break;
		case T_24:   templt->size=byte3  ; break;
		case T_32:   templt->size=byte4  ; break;
		case T_64:   templt->size=byte8  ; break;
		case T_WORD: templt->size=word   ; break;
		case T_MAX:  templt->size=max_t  ; break;
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
	sym_entry templt;
	
	//puts("Decl_Symbol");
	
	// Initialize the template
	memset((void*) &templt, 0, sizeof(sym_entry));
	
	Type_specifier(&templt);
	
	Initializer_list(&templt);
	
} // end Decl_Symbol


