/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "compiler.h"

/**	Create a Portable Executable file from the intermediate represetation
	Takes the output file name as a parameter. stack or register based VM?
 */
void pexe (char * filename, const DS blk_q){
	FILE* fd;
	
	if (verbosity) printf("Creating pexe file: %s\n", filename);
	fd = fopen(filename, "w");
	
	
	fclose(fd);
}
