/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#include "compiler.h"

//typedef enum {
//	F_OC = 0,
//	F_ROOT, ///< The infile name without the extension
//	F_DBG,
//	F_PEXE,
//	F_ASM,
//	F_OBJ,
//	NUM_TMP_FILES
//} tmp_files;


//void set_files(
//	char  ** (*filenames)[NUM_TMP_FILES],
//	yuck_t * argp,
//	char   * outfile
//){
//	unsigned int length;
//	char * in;
//	
//	// allocate space
//	for(int i=0; i<NUM_TMP_FILES; i++){
//		(*filenames)[i] = malloc(sizeof(char*) * argp->nargs);
//		if(! (*filenames)[i] ) crit_error("Out of memory");
//	}
//	
//	if(verbosity)printf("TMP_MAX: %d\n", TMP_MAX);
//	
//	// For each infile
//	for(uint i=0; i<argp->nargs; i++){
//		in = (*filenames)[F_OC][i] = argp->args[i];
//		
//		// get the filename root
//		length = strlen(in);
//		
//		if(in[length-2] == 'c' && in[length-3] == 'o' && in[length-4] == '.'){
//			(*filenames)[F_ROOT][i] = calloc(1, length-2);
//			if (! (*filenames)[F_ROOT][i]) crit_error("Out of memory");
//			
//			strncpy((*filenames)[F_ROOT][i], in, length-3);
//		}
//		else (*filenames)[F_ROOT][i] = (*filenames)[F_OC][i];
//		
//		// Set the temporary files
//		for(int j=F_DBG; j <= F_OBJ; j++){
//			(*filenames)[j][i] = malloc(L_tmpnam);
//			if (! (*filenames)[j][i] ) crit_error("Out of memory");
//			if (! tmpnam((*filenames)[j][i]) ) crit_error("tmpnam() failed");
//		}
//	}
//	
//	// Set the outfile
//	if(argp->outfile_arg) outfile = argp->outfile_arg;
//	if (verbosity)
//		printf("Setting %s as the output file.\n", outfile);
//	
//	puts("");
//}


static void Initialize(yuck_t * arg_pt){
	if (arg_pt->nargs > 1) puts("Too many arguments...Ignoring.");
	
	// set the global verbosity
	verbosity=arg_pt->dashv_flag;
	if(verbosity) printf("\
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
	
	// initialize the symbol table
	symbols = DS_new(
		DS_bst,
		sizeof(sym_entry),
		false,
		&cmp_sym,
		&cmp_sym_key
	);
	
	// initialize the intermediate code queues
	global_inst_q = DS_new(
		DS_list,
		sizeof(icmd),
		false,
		NULL,
		NULL
	);
	
	sub_inst_q = DS_new(
		DS_list,
		sizeof(icmd),
		false,
		NULL,
		NULL
	);
	
	nxt_lbl       = 0; // make sure the nxt_lbl is empty
	
}


static void Parse(yuck_t * arg_pt){
	bool errors;
	
	// Set the infile
	if(arg_pt->nargs){
		yyin = fopen(*arg_pt->args, "r");
		if(!yyin) crit_error("No such file");
		printf("Reading from: %s\n", *arg_pt->args);
	}
	else puts("Reading from: stdin");
	
	// Set the debug file
	if (arg_pt->dashd_flag){
		char *dbgfile;
		
		dbgfile = malloc(strlen(*arg_pt->args) + strlen(default_dbg) + 1);
		if(! dbgfile) crit_error("Out of Memory");
		
		dbgfile = strcpy(dbgfile, *arg_pt->args);
		dbgfile = strncat(dbgfile, default_dbg, strlen(default_dbg));
		debug_fd = fopen(dbgfile, "w");
		
		fprintf(debug_fd,"#Omnicode Intermediate File\n");
		
		free(dbgfile);
	}
	
	get_token(); // Initialize the lookahead token
	
	errors=setjmp(anewline); // Save the program state for error recovery
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	// Close the infile
	fclose(yyin);
	
	// close the debug file
	if (debug_fd){
		fputs("\n\n", debug_fd);
		Dump_symbols();
		fclose(debug_fd);
	}
	
	if(errors){
		puts("Errors were found.");
		exit(EXIT_FAILURE);
	}
}


static void Optomize(void){}


static void Generate_code(yuck_t * arg_pt){
	
	// pexe
	if (arg_pt->dashp_flag){
		char *pexefile;
		
		
		pexefile = malloc(strlen(*arg_pt->args) + strlen(default_pexe) + 1);
		if(! pexefile) crit_error("Out of Memory");
		
		pexefile = strcpy(pexefile, *arg_pt->args);
		pexefile = strncat(pexefile, default_pexe, strlen(default_pexe));
		
		pexe(pexefile);
		
		free(pexefile);
	}
	
	// asm
	if (arg_pt->dasha_flag || !arg_pt->dashp_flag){
		char *asmfile;
		
		if (arg_pt->nargs){
			asmfile = malloc(strlen(*arg_pt->args) + strlen(default_asm) + 1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, *arg_pt->args);
		}
		else{
			asmfile = malloc(strlen(default_out) + strlen(default_asm) + 1);
			if(! asmfile) crit_error("Out of Memory");
			asmfile = strcpy(asmfile, default_out);
		}
		
		asmfile = strncat(asmfile, default_asm, strlen(default_asm));
		
		// TODO: call code generator
		
		free(asmfile);
	}
}


static void Cleanup(yuck_t * arg_pt){
	yuck_free(arg_pt);
	DS_delete(symbols);
	DS_delete(global_inst_q);
	DS_delete(sub_inst_q);
}


int main (int argc, char** argv){
	yuck_t arg_pt[1];
	
	yuck_parse(arg_pt, argc, argv);
	Initialize(arg_pt);
	Parse(arg_pt);
	Optomize();
	Generate_code(arg_pt);
	Cleanup(arg_pt);
	
	return EXIT_SUCCESS;
}


