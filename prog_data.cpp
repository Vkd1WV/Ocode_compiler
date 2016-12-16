/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/ 


#include "prog_data.h"

// string length limit for unique compiler generated labels
// sufficiently large for 32-bit numbers in decimal and then some.
#define UNQ_NAME_SZ 16


Program_data::~Program_data(void){
	DS_pt blk_pt;
	
	debug_msg("Deleting the name array");
	free(string_array);
	
	
	debug_msg("Deleting the blocks");
	sprintf(err_array, "There are %u blocks", DS_count(block_q));
	debug_msg(err_array);
	
	while(( blk_pt = (DS_pt) DS_dq(block_q) )){
		#ifdef BLK_ADDR
		sprintf(err_array, "got block: %p", (void*) (*blk_pt));
		debug_msg(err_array);
		#endif
		DS_delete(*blk_pt);
		//debug_msg("deleted");
	}
	
	debug_msg("Deleting the block queue");
	DS_delete(block_q);
	debug_msg("Deleting the symbol table");
	DS_delete(symbols);
}


str_dx Program_data::add_string(const char * name){
	str_dx name_sz;
	str_dx temporary;
	
	/* we have to keep track of things with an index to ensure that the data
	will continue to be accessable when the array is resized.
	*/
	
	name_sz = (str_dx)strlen(name)+1; // +1 for the null
	
	// resize the array if necessary
	if(sa_size - sa_next < name_sz){
		if ((unsigned long int)sa_size * 2 > UINT_MAX )
			crit_error("Name_array maxed out");
		string_array = (char*) realloc(string_array, (sa_size *= 2));
		if(!string_array) crit_error("Out of Memory");
	}
	
	// Add name
	strncpy(access_string(sa_next), name, name_sz);
	temporary = sa_next;
	sa_next += name_sz;
	return temporary;
}


str_dx Program_data::new_label(void){
	static umax i;
	static char label[UNQ_NAME_SZ];
	// sufficiently large for 32-bit numbers in decimal and then some.
	
	sprintf(label, "_$%04lld", i++); // use $ to prevent collisions
	return add_string(label);
}


/**	create a new symbol table entry with a unique name.
 *	*	temporary symbol names all begin with %.
 */
sym_pt Program_data::new_var(sym_type type){
	static umax i;
	char name[UNQ_NAME_SZ];
	static struct sym new_symbol; // initialized to 0
	
	// give it a unique name
	sprintf(name, "_#%04lld", i++);
	new_symbol.name = add_string(name);
	new_symbol.type = type;
	new_symbol.temp = true;
	
	if(new_symbol.stat || new_symbol.constant || new_symbol.init || new_symbol.size) puts("new_var() screwed up!!");
	
	// insert it into the symbol table and return it
	return (sym_pt)DS_insert(symbols, &new_symbol);
}


void Program_data::remove_sym(str_dx dx){
	if( DS_find(symbols, get_string(dx)) ) DS_remove(symbols);
	else err_msg("emit_iop(): Internal: couldn't find symbol");
}


void Program_data::Print_sym(FILE * fd, sym_pt sym) const {
	const char * types[st_NUM] = {
		"undef", "int", "ref", "fun", "sub", "lit_int", "lit_str", "tp_def"
	};
	const char * widths[w_NUM] = {
		"undef", "byte", "byte2", "word", "byte4", "max", "byte8"
	};
	
	if(!sym) fputs("NULL\n", fd);
	else
		fprintf(fd, "%6s:\t%7s %5s %s%s%s%s%s  %p\n",
			get_string(sym->name),
			types[sym->type],
			sym->type == st_int? widths[sym->size] : "",
			sym->temp    ? "t": " ",
			sym->constant? "c": " ",
			sym->stat    ? "s": " ",
			sym->set     ? "v": " ", // whether it currently has a value assigned
			sym->init    ? "i": " ", // wether it has a static initialization
			(void*) sym->dref
		);
}


// Dump the symbol Table
void Program_data::dump_symbols(FILE * fd) const {
	sym_pt sym;
	
	debug_msg("\tDump_symbols(): start");
	if (!fd) err_msg("Internal: Dump_symbols(): received NULL file descriptor");
	
	if (DS_count(symbols)){
		fputs("  Name:\t   Type Width Flags Dref\n", fd);
		
		sym = (sym_pt) DS_first(symbols);
		do {
			Print_sym(fd, sym);
		} while(( sym = (sym_pt) DS_next(symbols) ));
		
		#ifdef FLUSH_FILES
			fflush(fd);
		#endif
	}
	else{
		info_msg("\tDump_symbols(): The Symbol list is empty");
		fputs("Empty\n", fd);
	}
	
	debug_msg("\tDump_symbols(): stop");
}



//void Program_data::Dump_blkq(FILE * fd) const {
//	DS_pt blk_pt;
//	
//	debug_msg("\tDump_blkq(): start");
//	if (!fd) err_msg("Internal: Dump_blkq(): received NULL file descriptor");
//	
//	if(!DS_isempty(block_q)){
//		blk_pt = (DS_pt) DS_first(block_q);
//		fputs("LBL   :\tI_OP\t RESULT \t  ARG1  \t  ARG2", fd);
//		do {
//			if(!blk_pt) err_msg("\tDump_blkq(): found an empty block");
//			
//			else{
//				fputs("\n", fd);
//				Dump_iq(fd, *blk_pt);
//			}
//		} while(( blk_pt = (DS_pt) DS_next(blkq) ));
//	}
//	else {
//		info_msg("\tDump_blkq(): The block queue is empty");
//		fputs("Empty\n", fd);
//	}
//	
//	debug_msg("\tDump_blkq(): stop");
//}


