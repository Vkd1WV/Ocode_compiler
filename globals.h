#ifndef _GLOBALS_H
#define _GLOBALS_H

#ifdef _GLOBALS_C
#define EXTERN

#else
#define EXTERN extern

#endif


EXTERN token_t token;        // global lookahead token
EXTERN FILE*   outfile;      // parser output file
EXTERN DS      symbol_table;
EXTERN mode_t  x86_mode;     // the proccessor mode we are compiling for
EXTERN uint    block_lvl;    // no. of leading tabs on the current line passed by T_TAB
EXTERN umax    yynumber;     // numbers passed to the parser by T_NUM

EXTERN const char* _e_noimp;

// Global variables provided by the scanner
extern int     yylineno;
extern char *  yytext;

#endif // _GLOBALS_H
