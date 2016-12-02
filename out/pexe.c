/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"

typedef struct pexe_header{
	uint64_t magic        ; ///< Magic number
	uint32_t version      ; ///< File version
	uint32_t name_array_sz; ///< Name Array length in bytes
	///< File position for start of symbol table
	///< File position for start of op queue
	///< checksum of everything after the header
} pexe_h;

//typedef struct pexe_symbol{
//	
//} pexe_sym;

//typedef struct pexe_operation{
//	
//} pexe_op;


/**	Create a Portable Executable file from the intermediate represetation
	Takes the output file name as a parameter. stack or register based VM?
 */
void pexe (char * filename, const DS blk_q){
	FILE* fd;
	
	sprintf(err_array, "Creating pexe file: '%s'", filename);
	info_msg(err_array);
	fd = fopen(filename, "w");
	
	
	fclose(fd);
}
