/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "proto.h"
#include "errors.h"
#include <stdio.h>

typedef struct pexe_header{
	uint8_t  magic0;
	uint8_t  magic1;
	uint8_t  magic2;
	uint8_t  magic3;
	uint8_t  magic4;
	uint8_t  magic5;
	uint16_t version; ///< File version
	// 8 bytes total in file signature
	
	// locations and sizes of components
	uint32_t name_array_sz; ///< Name Array length in bytes
	uint32_t off_sym;       ///< File position for start of symbol table
	uint32_t off_instq;     ///< File position for start of op queue
	uint32_t padding;       ///< just to keep the fields alligned
	
	// file integity verification
	uint64_t checksum; ///< checksum of everything after the header
} pexe_h;

// File signature
const uint8_t  magic0 = 0xB4;
const uint8_t  magic1  = 'P';
const uint8_t  magic2  = 'E';
const uint8_t  magic3  = 'X';
const uint8_t  magic4 = 'E';
const uint8_t  magic5 = 0xAF;
const uint16_t version = 0x0000;

//typedef struct pexe_symbol{
//	
//} pexe_sym;

//typedef struct pexe_operation{
//	
//} pexe_op;


/**	Create a Portable Executable file from the intermediate represetation
	Takes the output file name as a parameter. stack or register based VM?
 */
void pexe (char * filename){
	FILE* fd;
	pexe_h header;
	size_t result;
	
	msg_print(NULL, V_INFO, "pexe(): start");
	
	//Initialize the header
	header.magic0  = magic0;
	header.magic1  = magic1;
	header.magic2  = magic2;
	header.magic3  = magic3;
	header.magic4  = magic4;
	header.magic5  = magic5;
	header.version = version;
	
	header.name_array_sz = 0;
	header.off_sym       = 0;
	header.off_instq     = 0;
	header.padding       = 0;
	
	header.checksum = 0;
	
	
	msg_print(NULL, V_INFO, "pexe(): Size of pexe_h is: %lu", sizeof(pexe_h));
	
	// open the file
	msg_print(NULL, V_INFO, "pexe(): Creating file: '%s'", filename);
	fd = fopen(filename, "w");
	if(!fd){
		msg_print(NULL, V_ERROR, "pexe(): Could not open file");
		return;
	}
	
	// write header
	result = fwrite(&header, 1, sizeof(pexe_h), fd);
	
	msg_print(NULL, V_INFO, "pexe(): fwrite() returned %lu", result);
	
	fclose(fd);
	fd=NULL;
	
	msg_print(NULL, V_INFO, "pexe(): stop");
}


