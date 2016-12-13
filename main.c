/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"
#include "yuck.h"

// Default output files
const char * default_dbg  = ".dbg" ;
const char * default_out  = "out"  ;
const char * default_asm  = ".asm" ;
const char * default_pexe = ".pexe";


static inline void Set_files(char ** infilename, yuck_t * arg_pt){
	uint   sum;
	char * debug_file;
	
	// default verbosity
	verbosity = DEFAULT_VERBOSITY + arg_pt->dashv_flag - arg_pt->dashq_flag;
	
	if (arg_pt->nargs > 1) warn_msg("Too many arguments...Ignoring.");
	
	if(verbosity >= V_DEBUG) fprintf(stderr, "\
ARGUMENTS PASSED\n\
nargs             :\t%lu\n\
args              :\t%s\n\
dashv_flag        :\t%u\n\
dashq_flag        :\t%u\n\
dashD_arg         :\t%s\n\
dashd_flag        :\t%u\n\
dashp_flag        :\t%u\n\
x86_long_flag     :\t%u\n\
x86_protected_flag:\t%u\n\
arm_v7_flag       :\t%u\n\
arm_v8_flag       :\t%u\n\n" ,
			arg_pt->nargs      ,
			*arg_pt->args      ,
			arg_pt->dashv_flag ,
			arg_pt->dashq_flag ,
			arg_pt->dashD_arg  ,
			arg_pt->dashd_flag ,
			arg_pt->dashp_flag ,
			arg_pt->x86_long_flag     ,
			arg_pt->x86_protected_flag,
			arg_pt->arm_v7_flag       ,
			arg_pt->arm_v8_flag
		);
	
	// test for a target architecture
	sum = arg_pt->x86_long_flag + arg_pt->x86_protected_flag;
	sum += arg_pt->arm_v7_flag + arg_pt->arm_v8_flag;
	
	// if too many targets
	if(sum > 1)
		crit_error("Must specify exactly one target architecture.");
	
	// if no target has been selected
	if(sum < 1 && !arg_pt->dashp_flag) {
		info_msg("Using default target x86-long");
		arg_pt->x86_long_flag = true;
	}
	
	// Set the infile
	if(arg_pt->nargs) *infilename = *arg_pt->args;
	else *infilename = NULL;
	
	// Set the debug file
	if (arg_pt->dashd_flag){
		make_debug = true;
		
		debug_file = (char*) malloc(strlen(*arg_pt->args)+strlen(default_dbg)+1);
		if(!debug_file) crit_error("Out of Memory");
		
		debug_file = strcpy(debug_file, *arg_pt->args);
		debug_file = strncat(debug_file, default_dbg, strlen(default_dbg));
		
		// clear the file
		fclose(fopen(debug_file, "w"));
		debug_fd = NULL;
		
		debug_fd = fopen(debug_file, "a");
		
		free(debug_file);
	}
	else make_debug = false;
}


static inline void Generate_code(yuck_t * arg_pt, Program_data * prog){
	uint sum;
	char *pexefile, *asmfile;
	
	info_msg("Generate_code(): start");
	
	// pexe
	if (arg_pt->dashp_flag){
		pexefile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_pexe)+1);
		if(! pexefile) crit_error("Out of Memory");
		
		pexefile = strcpy(pexefile, *arg_pt->args);
		pexefile = strncat(pexefile, default_pexe, strlen(default_pexe));
		
		sprintf(err_array, "pexefile is: '%s'", pexefile);
		info_msg(err_array);
		
		pexe(pexefile, prog);
		
		free(pexefile);
	}
	
	sum = arg_pt->x86_long_flag + arg_pt->x86_protected_flag;
	sum += arg_pt->arm_v7_flag + arg_pt->arm_v8_flag;
	
	// asm
	if (sum){
		// set the assembler output file
		if (arg_pt->nargs){
			asmfile=(char*) malloc(strlen(*arg_pt->args)+strlen(default_asm)+1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, *arg_pt->args);
		}
		else{
			asmfile = (char*) malloc(strlen(default_out)+strlen(default_asm)+1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, default_out);
		}
		
		asmfile = strncat(asmfile, default_asm, strlen(default_asm));
		
		sprintf(err_array, "asmfile is: '%s'", asmfile);
		info_msg(err_array);
		
		if(arg_pt->x86_long_flag) x86(asmfile, prog, true);
		else if(arg_pt->x86_protected_flag) x86(asmfile, prog, false);
		else err_msg("Unimplemented Target");
		
		free(asmfile);
	}
	
	info_msg("Generate_code(): stop");
}


int main (int argc, char** argv){
	yuck_t arg_pt[1];
	char * infile;
	Program_data prog_data[1];
	
	// Setup
	yuck_parse(arg_pt, argc, argv);
	Set_files(&infile, arg_pt);
	Init_program_data(prog_data);
	
	// Compile
	if( Parse(prog_data, infile) ){
		err_msg("Exiting");
		yuck_free(arg_pt);
		if(make_debug) fclose(debug_fd);
		debug_fd = NULL;
		Clear_program_data(prog_data);
		return EXIT_FAILURE;
	}
	
	if (make_debug) Dump_second(debug_fd, prog_data);
	
	Generate_code(arg_pt, prog_data);
	
	// Cleanup
	yuck_free(arg_pt);
	if(make_debug) fclose(debug_fd);
	debug_fd = NULL;
	Clear_program_data(prog_data);
	
	return EXIT_SUCCESS;
}


