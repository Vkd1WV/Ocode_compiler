#ifndef _GLOBALS_H
#define _GLOBALS_H

#ifdef _GLOBALS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/*************************** PARSING / SCANNING *******************************/

EXTERN token_t token;     // global lookahead token
EXTERN uint    block_lvl; // number of leading tabs on the current line
EXTERN umax    yynumber;  // numbers passed to the parser by T_NUM
EXTERN jmp_buf anewline;  // to facilitate error recovery

// Global variables provided by the scanner
#ifndef _SCANNER_L
extern int     yylineno;
extern char *  yytext;
extern FILE *  yyin;
#endif

/**************************** INTERMEDIATE CODE *******************************/

EXTERN DS    global_symbols; // symbol table for global symbols
EXTERN DS    interm_q;       // a queue for intermediate code
EXTERN FILE* debug_fd;       // contains a text representation of the interm_q
EXTERN char* nxt_lbl;        // points to the label of the next icmd
extern const char * byte_code_dex[50]; // initialized in globals.c

/***************************** CODE GENERATOR *********************************/

EXTERN FILE*   out_fd;
EXTERN mode_t  x86_mode; // the proccessor mode we are compiling for
EXTERN arch_t  arch;     // the target architecture

// defined in globals.c
//extern const char* _e_noimp;

#endif // _GLOBALS_H
