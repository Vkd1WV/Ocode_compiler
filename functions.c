

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
	sprintf(new_symbol->name, "%%%04lld", i++);
	
	// insert it into the symbol table
	sort(global_symbols, new_symbol, new_symbol->name);
	
	// Copy stuff
	new_symbol->type = temp;
	
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
	char arg1[NAME_MAX];
	
	if (in->constant) sprintf(arg1, "#%4llx", in->value);
	else              sprintf(arg1, "%s", in->name);
	
	fprintf(outfile, "%s\t%s\t%s\n", cmd, out->name, arg1);
}

void emit_quad(
	const char* cmd,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
){
	char arg1[NAME_MAX], arg2[NAME_MAX];
	
	if (left->constant) sprintf(arg1, "#%4llx", left->value);
	else                sprintf(arg1, "%s", left->name);
	
	if (right->constant) sprintf(arg2, "#%4llx", right->value);
	else                 sprintf(arg2, "%s", right->name);
	
	fprintf(outfile, "%s\t%s\t%s\t%s\n", cmd, out->name, arg1, arg2);
}

/*void Move(reg_t dest, regsz_t dsize, reg_t src, regsz_t ssize){*/
/*	char output[22] = "mov     x,   x";*/
/*	*/
/*	if(x86_mode<Long && (dsize>dword || ssize>dword))*/
/*		error("operand too large for current proccessor mode");*/
/*	if(x86_mode<Long && ((dest>R7 && dest<=R15) || (src>R7 && src<=R15)))*/
/*		error("registers R8-R15 are only availible in Long mode");*/
/*	*/
/*	*/
/*	if(dsize > ssize)*/
/*		// extend*/
/*		{}*/
/*	else if (dsize < ssize)*/
/*		// better be a recast*/
/*		{}*/
/*	*/
/*	switch (dsize){*/
/*	case byte:*/
/*		output[8] = 'l';*/
/*		break;*/
/*	case dword:*/
/*		output[6] = 'e';*/
/*		break;*/
/*	case qword:*/
/*		output[6] = 'r';*/
/*		break;*/
/*	default:*/
/*		error("something done broke in Move()");*/
/*	}*/
/*	*/
/*	switch (ssize){*/
/*	case byte:*/
/*		output[13] = 'l';*/
/*		break;*/
/*	case dword:*/
/*		output[11] = 'e';*/
/*		break;*/
/*	case qword:*/
/*		output[11] = 'r';*/
/*		break;*/
/*	default:*/
/*		error("something done broke in Move()");*/
/*	}*/
/*	*/
/*	switch (src){*/
/*	case R0:*/
/*	case A:*/
/*		output[12]='a';*/
/*		break;*/
/*	case R1:*/
/*	case B:*/
/*		output[12]='b';*/
/*		break;*/
/*	case R2:*/
/*	case C:*/
/*		output[12]='c';*/
/*		break;*/
/*	case R3:*/
/*	case D:*/
/*		output[12]='d';*/
/*		break;*/
/*	case R4:*/
/*	case SI:*/
/*		output[12]='s';*/
/*		output[13]='i';*/
/*		break;*/
/*	case R5:*/
/*	case DI:*/
/*		output[12]='d';*/
/*		output[13]='i';*/
/*		break;*/
/*	case R6:*/
/*	case BP:*/
/*		output[12]='b';*/
/*		output[13]='p';*/
/*		break;*/
/*	case R7:*/
/*	case SP:*/
/*		output[12]='s';*/
/*		output[13]='p';*/
/*		break;*/
/*	}*/
/*	*/
/*	switch (dest){*/
/*	case R0:*/
/*	case A:*/
/*		output[7]='a';*/
/*		break;*/
/*	case R1:*/
/*	case B:*/
/*		output[7]='b';*/
/*		break;*/
/*	case R2:*/
/*	case C:*/
/*		output[7]='c';*/
/*		break;*/
/*	case R3:*/
/*	case D:*/
/*		output[7]='d';*/
/*		break;*/
/*	case R4:*/
/*	case SI:*/
/*		output[7]='s';*/
/*		output[8]='i';*/
/*		break;*/
/*	case R5:*/
/*	case DI:*/
/*		output[7]='d';*/
/*		output[8]='i';*/
/*		break;*/
/*	case R6:*/
/*	case BP:*/
/*		output[7]='b';*/
/*		output[8]='p';*/
/*		break;*/
/*	case R7:*/
/*	case SP:*/
/*		output[7]='s';*/
/*		output[8]='p';*/
/*		emit_cmnt("Probably shouldn't be writing to SP");*/
/*		break;*/
/*	}*/
/*	emit_cmd(output);*/
/*}*/

