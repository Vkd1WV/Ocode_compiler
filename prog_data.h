/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _INTERMED_H
#define _INTERMED_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "prog_def.h"


/******************************************************************************/
//                            GLOBAL CONSTANTS
/******************************************************************************/


#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array
#define NO_NAME     ((str_dx)UINT_MAX)

#define START_LBL "_#START"


/******************************************************************************/
//                          GLOBAL INLINE FUNCTIONS
/******************************************************************************/

//extern char * name_array;

///// find a name in the name_array by its str_dx
//static inline char * dx_to_name(str_dx index){
//	if(index == NO_NAME) return NULL;
//	else return name_array+index;
//}



/********************** PRINT INTERMEDIATE REPRESENTATION *********************/



/**************** DUMP INTERMEDIATE REPRESENTATION TO A FILE ******************/







//static inline void Dump_second(FILE * fd, Program_data * prog){
//	
//	if(!make_debug) return;
//	
//	info_msg("Dump_second(): start");
//	
//	if(!fd) {
//		warn_msg("Internal: Dump_second(): no such file");
//		return;
//	}
//	
//	fputs("\n== AFTER OPTOMIZATION ==\n", fd);
//	
//	fputs("\nSymbol Table\n", fd);
//	Dump_symbols(fd, prog->symbols);
//	
//	fputs("\nBlock Queue\n", fd);
//	Dump_blkq(fd, prog->block_q);
//	
//	info_msg("Dump_second(): stop");
//}

/************************** INITIALIZE AND DELETE *****************************/

//static inline void Init_program_data(Program_data * data_pt){
//	sprintf(err_array, "sizeof(DS): %lu", sizeof(DS));
//	debug_msg(err_array);
//	sprintf(err_array, "sizeof(icmd): %lu", sizeof(icmd));
//	debug_msg(err_array);
//	sprintf(err_array, "sizeof(struct sym): %lu", sizeof(struct sym));
//	debug_msg(err_array);

//	data_pt->block_q = (DS) DS_new_list(sizeof(DS));
////	data_pt->main_q  = (DS) DS_new_list(sizeof(icmd));
////	data_pt->sub_q   = (DS) DS_new_list(sizeof(icmd));
//	data_pt->symbols = (DS) DS_new_bst(
//		sizeof(struct sym),
//		false,
//		&sym_key,
//		&cmp_sym
//	);
//}

//static inline void Clear_program_data(Program_data * data_pt){
//	DS_pt blk_pt;

//	debug_msg("Deleting the name array");
//	free(data_pt->names);
//	
//	
//	debug_msg("Deleting the blocks");
//	sprintf(err_array, "There are %u blocks", DS_count(data_pt->block_q));
//	debug_msg(err_array);
//	
//	while(( blk_pt = (DS_pt) DS_dq(data_pt->block_q) )){
//		#ifdef BLK_ADDR
//		sprintf(err_array, "got block: %p", (void*) (*blk_pt));
//		debug_msg(err_array);
//		#endif
//		DS_delete(*blk_pt);
//		//debug_msg("deleted");
//	}
//	
//	debug_msg("Deleting the block queue");
//	DS_delete(data_pt->block_q);
//	
////	debug_msg("Deleting the main Queue");
////	DS_delete(data_pt->main_q);
////	debug_msg("Deleting the Sub queue");
////	DS_delete(data_pt->sub_q);
//	debug_msg("Deleting the symbol table");
//	DS_delete(data_pt->symbols);
//}


/******************************************************************************/
//                            CLASS DEFINITION
/******************************************************************************/


// All program data is stored here
class Program_data{
private:
	// DATA
	// These are static because only one Program_data can exist at a time
	static char * string_array;
	static str_dx sa_size;
	static str_dx sa_next;
	
	static DS     block_q;
	static DS     symbols;
	
	
	// FUNCTIONS
private:
	// Used in symbols only
	static inline int cmp_sym(const void * left, const void * right){
		return strcmp( (char*)left, (char*)right );
	}
	static inline const void * sym_key(const void * symbol){
		return (const void *) (string_array+(sym_pt (symbol)->name));
	}
	
	// returns non-constant
	inline char * access_string(str_dx dx) const {
		if(dx == NO_NAME) return NULL;
		else return string_array+dx;
	}
	inline sym_pt find_sym(str_dx dx) const {
		return (sym_pt)DS_find(symbols, get_string(dx));
	}
	
	void Print_sym (FILE * fd, sym_pt sym) const;
	
public:
	Program_data(void){
		// Initialize the string array
		string_array = (char*)malloc(sizeof(char) * NAME_ARR_SZ);
		if(!string_array) crit_error("Out of Memory");
		sa_size = NAME_ARR_SZ;
		
		block_q = (DS) DS_new_list(sizeof(DS));
		symbols = (DS) DS_new_bst(
			sizeof(struct sym),
			false,
			&Program_data::sym_key,
			&Program_data::cmp_sym
		);
	}
	~Program_data(void);
	
	// Mutators
	str_dx add_string(const char * name);
	str_dx new_label (void);
	sym_pt new_var   (sym_type type);
	void   remove_sym(str_dx dx);
	
	// Accessors
	inline const char * get_string(str_dx dx) const {
		return (const char *) access_string(dx);
	}
	void dump_symbols(FILE * fd) const;
};


#endif // _INTERMED_H


