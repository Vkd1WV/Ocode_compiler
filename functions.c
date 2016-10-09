

#include "compiler.h"

// create and return a pointer to a unique label
const char* new_label(void){
	static umax i;
	static char label[UNQ_LABEL_SZ];
	
	sprintf(label, "__%04lld", i++); // use __ to prevent collisions
	return label;
}

// create and return a pointer to a new symbol table entry
sym_entry* new_var(void){
	static umax i;
	
	// create a new symbol entry
	sym_entry* new_symbol=calloc(1, sizeof(sym_entry));
	if (!new_symbol) error("Out of memory");
	
	// give it a unique name
	sprintf(new_symbol->name, "__%04lld", i++);
	
	// insert it into the symbol table
	sort(global_symbols, new_symbol, new_symbol->name);
	
	// and return it
	return new_symbol;
}

// return a pointer to a string representing a pointer
const char* pointertt(const sym_entry * p){
	static char p_text[20]; // some value large enough
	
	sprintf(p_text, "%p", (void*)p);
	return p_text;
}

void emit_triple(
	const char* cmd,
	const sym_entry* out,
	const sym_entry* in
){
	char arg1[PT_SIZE];
	
	if (in->constant) sprintf(arg1, "# %16llx", in->value);
	else              sprintf(arg1, "%18p", (void*) in);
	
	fprintf(outfile, "%s\t%18p\t%s\n", cmd, (void*)out, arg1);
}

void emit_quad(
	const char* cmd,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
){
	char arg1[PT_SIZE], arg2[PT_SIZE];
	
	if (left->constant) sprintf(arg1, "# %16llx", left->value);
	else                sprintf(arg1, "%18p", (void*) left);
	
	if (right->constant) sprintf(arg2, "# %16llx", right->value);
	else                 sprintf(arg2, "%18p", (void*) right);
	
	fprintf(outfile, "%s\t%18p\t%s\t%s\n", cmd, (void*)out, arg1, arg2);
}


