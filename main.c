/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "compiler.h"


static void Initialize(yuck_t * arg_pt){
	uint sum;
	
	if (arg_pt->nargs > 1) puts("Too many arguments...Ignoring.");
	
	// set the global verbosity
	switch(arg_pt->dashv_flag){
	case 2: verbosity = V_DEBUG; break;
	case 1: verbosity = V_INFO ; break;
	case 0:
	default: verbosity = V_NOTE; break;
	}
	
	
	if(verbosity >= V_DEBUG) printf("\
ARGUMENTS PASSED\n\
nargs             :\t%lu\n\
args              :\t%s\n\
dashv_flag        :\t%u\n\
dashD_arg         :\t%s\n\
dashd_flag        :\t%u\n\
dashp_flag        :\t%u\n\
dasha_flag        :\t%u\n\
x86_long_flag     :\t%u\n\
x86_protected_flag:\t%u\n\
arm_v7_flag       :\t%u\n\
arm_v8_flag       :\t%u\n\n" ,
			arg_pt->nargs      ,
			*arg_pt->args      ,
			arg_pt->dashv_flag ,
			arg_pt->dashD_arg  ,
			arg_pt->dashd_flag ,
			arg_pt->dashp_flag ,
			arg_pt->dasha_flag ,
			arg_pt->x86_long_flag     ,
			arg_pt->x86_protected_flag,
			arg_pt->arm_v7_flag       ,
			arg_pt->arm_v8_flag
		);
	
	// test for a target architecture
	sum = arg_pt->x86_long_flag + arg_pt->x86_protected_flag;
	sum += arg_pt->arm_v7_flag + arg_pt->arm_v8_flag;
	
	if(arg_pt->dasha_flag && sum != 1)
		crit_error("Must specify exactly one target architecture.");
	
	// initialize the symbol table
	symbols = DS_new_bst(
		sizeof(struct sym),
		false,
		&sym_key,
		&cmp_sym
	);
	
	// initialize the intermediate code queues
	global_inst_q = DS_new_list(sizeof(icmd));
	sub_inst_q    = DS_new_list(sizeof(icmd));
}


static void Generate_code(yuck_t * arg_pt, DS blk_q){
	
	// pexe
	if (arg_pt->dashp_flag){
		char *pexefile;
		
		pexefile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_pexe)+1);
		if(! pexefile) crit_error("Out of Memory");
		
		pexefile = strcpy(pexefile, *arg_pt->args);
		pexefile = strncat(pexefile, default_pexe, strlen(default_pexe));
		
		if(verbosity) printf("pexefile is: %s\n", pexefile);
		
		pexe(pexefile, blk_q);
		
		free(pexefile);
	}
	
	// asm
	if (arg_pt->dasha_flag || !arg_pt->dashp_flag){
		char *asmfile;
		
		if (arg_pt->nargs){
			asmfile = (char*) malloc(strlen(*arg_pt->args)+strlen(default_asm)+1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, *arg_pt->args);
		}
		else{
			asmfile = (char*) malloc(strlen(default_out)+strlen(default_asm)+1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, default_out);
		}
		
		asmfile = strncat(asmfile, default_asm, strlen(default_asm));
		
		if(verbosity) printf("asmfile is: %s\n", asmfile);
		
		if(arg_pt->x86_long_flag) x86(asmfile, true, blk_q);
		else if(arg_pt->x86_protected_flag) x86(asmfile, false, blk_q);
		
		free(asmfile);
	}
}


static void Cleanup(yuck_t * arg_pt, DS structure){
	DS blk;

	if(verbosity) puts("Cleanup");
	yuck_free(arg_pt);
	DS_delete(symbols);
	DS_delete(global_inst_q);
	DS_delete(sub_inst_q);
	
	while(( blk = (DS) DS_first(structure) )) DS_delete(blk);
	DS_delete(structure);
	
	free(name_array);
}


int main (int argc, char** argv){
	yuck_t arg_pt[1];
	DS blk_q;
	
	yuck_parse(arg_pt, argc, argv);
	Initialize(arg_pt);
	Parse(arg_pt);
	
	blk_q = Optomize(global_inst_q, sub_inst_q);
	
	Generate_code(arg_pt, blk_q);
	
	Cleanup(arg_pt, blk_q);
	
	return EXIT_SUCCESS;
}


