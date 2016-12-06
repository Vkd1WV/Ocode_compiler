/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "global.h"

// Default output files
const char * default_dbg  = ".dbg" ;
const char * default_out  = "out"  ;
const char * default_asm  = ".asm" ;
const char * default_pexe = ".pexe";


static inline void Set_files(FILE ** in_fd, FILE ** debug_fd, yuck_t * arg_pt){
	uint sum;
	char *dbgfile;
	
	// default verbosity
	verbosity = DEFAULT_VERBOSITY + arg_pt->dashv_flag - arg_pt->dashq_flag;
	
	if (arg_pt->nargs > 1) warn_msg("Too many arguments...Ignoring.");
	
	if(verbosity >= V_DEBUG) printf("\
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
	if(arg_pt->nargs){
		*in_fd = fopen(*arg_pt->args, "r");
		if(!*in_fd) crit_error("No such file");
		sprintf(err_array, "Reading from: %s\n", *arg_pt->args);
		info_msg(err_array);
	}
	else {
		*in_fd = stdin;
		info_msg("Reading from: stdin");
	}
	
	// Set the debug file
	if (arg_pt->dashd_flag){
		dbgfile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_dbg)+1);
		if(! dbgfile) crit_error("Out of Memory");
		
		dbgfile = strcpy(dbgfile, *arg_pt->args);
		dbgfile = strncat(dbgfile, default_dbg, strlen(default_dbg));
		*debug_fd = fopen(dbgfile, "w");
		
		free(dbgfile);
	}
	
}


static inline void Generate_code(yuck_t * arg_pt, Program_data prog){
	uint sum;
	char *pexefile, *asmfile;
	
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
}


int main (int argc, char** argv){
	yuck_t arg_pt[1];
	FILE *in_fd, *debug_fd;
	Program_data prog_data;
	
	yuck_parse(arg_pt, argc, argv);
	Set_files(&in_fd, &debug_fd, arg_pt);
	
	Init_program_data(&prog_data);
	
	Parse(prog_data, in_fd);
	
	if (debug_fd)
		Dump_parser(debug_fd, prog_data);
	
	Optomize(prog_data);
	
	if (debug_fd) Dump_blkq(debug_fd, prog_data);
	
	info_msg("Closing debug file");
	fclose(debug_fd);
	
	Generate_code(arg_pt, prog_data);
	
	info_msg("Cleanup...");
	yuck_free(arg_pt);
	Clear_program_data(prog_data);
	
	return EXIT_SUCCESS;
}


