/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

/**	Create a Portable Executable file from the intermediate represetation
	Takes the output file name as a parameter.
 */
bool pexe (char * filename){
	FILE* fd;
	
	if (verbose) printf("Creating pexe file: %s\n", filename);
	fd = fopen(filename, "w");
	
	
	
	return EXIT_SUCCESS;
}
