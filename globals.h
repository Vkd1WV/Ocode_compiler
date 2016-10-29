#ifndef _GLOBALS_H
#define _GLOBALS_H

#ifdef _GLOBALS_C
#define EXTERN

#else
#define EXTERN extern

#endif


EXTERN token_t token;        // global lookahead token
EXTERN FILE*   outfile;      // parser output file
EXTERN DS      global_symbols; // symbol table for global symbols
EXTERN mode_t  x86_mode;     // the proccessor mode we are compiling for
EXTERN arch_t  arch;         // the target architecture
EXTERN uint    block_lvl;    // no. of leading tabs on the current line.
// block_lvl is controlled entirely from scanner.l
EXTERN umax    yynumber;     // numbers passed to the parser by T_NUM
jmp_buf anewline; // to facilitate error recovery

// defined in globals.c
extern const char* _e_noimp;

// Global variables provided by the scanner
extern int     yylineno;
extern char *  yytext;

#endif // _GLOBALS_H
