/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include <string.h>

extern "C"{
	#include "yuck.h"
}

#include "errors.h"
#include <util/types.h>
#include "prog_data.h"
#include "proto.h"
#include "parse.h"

// Default output files
const char * default_dbg  = ".dbg" ;
const char * default_out  = "out"  ;
const char * default_asm  = ".asm" ;
const char * default_pexe = ".pexe";


static inline void set_log_level(yuck_t * arg_pt){
	msg_log_lvl v;
	
	// calculate log level
	v = (msg_log_lvl) (DEFAULT_VERBOSITY + (arg_pt->dashv_flag*2) - arg_pt->dashq_flag);
	
	if(v > V_TRACE) v = V_TRACE;
	else if(v<0) v = V_QUIET;
	
	msg_set_verbosity(v);
	msg_print(logfile, V_INFO, "Log level set to %u",v);
	
	if (arg_pt->nargs > 1) msg_print(NULL, V_WARN, "Too many arguments...Ignoring.");
	
	msg_print(logfile, V_DEBUG, "\
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
}


static inline void set_files(char ** infilename, yuck_t * arg_pt){
	uint   sum;
	char * debug_file = NULL;
	
	msg_print(logfile, V_TRACE, "set_files():start");
	
	// test for a target architecture
	sum = arg_pt->x86_long_flag + arg_pt->x86_protected_flag;
	sum += arg_pt->arm_v7_flag + arg_pt->arm_v8_flag;
	
	// if too many targets
	if(sum > 1)
		crit_error("Must specify exactly one target architecture.");
	
	// if no target has been selected
	if(sum < 1 && !arg_pt->dashp_flag) {
		msg_print(NULL, V_INFO, "Using default target x86-long");
		arg_pt->x86_long_flag = true;
	}
	
	// Set the infile
	if(arg_pt->nargs) *infilename = *arg_pt->args;
	else *infilename = NULL;
	
	// Set the debug file
	if (arg_pt->dashd_flag){
		make_debug = true;
		
		// FIXME: debug file from stdin causes segfault in strlen
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
	
	msg_print(logfile, V_TRACE, "set_files():stop");
}


static inline void Generate_code(yuck_t * arg_pt){
	uint sum;
	char *pexefile, *asmfile;
	
	msg_print(NULL, V_INFO, "Generate_code(): start");
	
	// pexe
	if (arg_pt->dashp_flag){
		pexefile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_pexe)+1);
		if(! pexefile) crit_error("Out of Memory");
		
		pexefile = strcpy(pexefile, *arg_pt->args);
		pexefile = strncat(pexefile, default_pexe, strlen(default_pexe));
		
		sprintf(err_array, "pexefile is: '%s'", pexefile);
		msg_print(NULL, V_INFO, "%s", err_array);
		
		pexe(pexefile);
		
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
		
		msg_print(NULL, V_INFO, "asmfile is: '%s'", asmfile);
		
		if(arg_pt->x86_long_flag) x86(asmfile, true);
		else if(arg_pt->x86_protected_flag) x86(asmfile, false);
		else msg_print(NULL, V_ERROR, "Unimplemented Target");
		
		free(asmfile);
	}
	
	msg_print(NULL, V_INFO, "Generate_code(): stop");
}


int main (int argc, char** argv){
	yuck_t arg_pt[1];
	char * infile;
	Program_data prog_data;
	bool errors=0;
	
	// Setup
	yuck_parse(arg_pt, argc, argv);
	set_log_level(arg_pt);
	set_files(&infile, arg_pt);
	
	
	// Compile
	errors = Parse(infile);
	
	if(make_debug){
		fputs("\n== AFTER OPTOMIZATION ==\n", debug_fd);
		prog_data.Dump(debug_fd);
	} 
	
	if(!errors) Generate_code(arg_pt);
	
	// Cleanup
	yuck_free(arg_pt);
	if(make_debug) fclose(debug_fd);
	debug_fd = NULL;
	
	return errors;
}


