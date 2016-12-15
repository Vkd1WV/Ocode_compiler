%{
/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#define _SCANNER_L // Used in globals.h to eliminate spurrious warnings

#include "global.h"
#include "scanner.h"
#define YY_DECL token_t yylex(void) // Eliminates spurrious warnings

/*
This function returns the correct symbol for the given name in the current scope
*/
static inline Bind(void){
	static char * buffer;
	static size_t buf_l;
	#define BUF_SZ 64
	
	char * prefix;
	sym_pt sym=NULL;
	size_t name_l;
	prg_blk * block_pt;
	
	// initializate the buffer
	if(!buffer){
		buffer = (char*)malloc(BUF_SZ);
		buf_l = BUF_SZ;
	}
	
	block_pt = (prg_blk*)DS_first(scope_stack);
	
	// resize the buffer if necessary
	if(block_pt->scope){
		prefix = dx_to_name(block_pt->scope->name);
		name_l = strlen(prefix) + strlen(yytext) + 1;
		if(name_l > buf_l) buffer = realloc(buffer, name_l);
	}
	
	// search the scope stack
	while ( block_pt->scope ){
		prefix = dx_to_name(block_pt->scope->name);
		strncpy(buffer, prefix, strlen(prefix));
		strncat(buffer, yytext, strlen(yytext));
		if(( sym = DS_find(symbols, buffer) )) break;
		block_pt = (prg_blk*)DS_next(scope_stack);
	}
	
	// if we didn't find anything, check the global scope
	if(!sym) sym = DS_find(symbols, yytext);
	
	return sym;
}

%}


%option noyywrap
%option yylineno
%x INCLUDE
%x STRING
%x CHAR

DIGIT  [0-9]
HEX    0x({DIGIT}|[a-f])+
LETTER [A-Za-z_]
NAME   {LETTER}({DIGIT}|{LETTER})*
NEWL   [ \t]*(\r\n|\n)
WHTSPC [ \t]+


/******************************************************************************/
/*                                  RULES*/
/******************************************************************************/
%%

	/* WHTSPC must come after the tab counter */
^[ *\t+] { // count leading tabs and pass in block_lvl
	for(int i=0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\t') block_lvl++;
}
{WHTSPC}  {} // whitespace

#.*{NEWL} { block_lvl=0; } // comments
^({NEWL}) { block_lvl=0; }  //ignore empty lines
{NEWL}   block_lvl=0; return T_NL;


 /* handle file inclusions */
include{WHTSPC} { BEGIN(INCLUDE); } // file include
<INCLUDE>[^ \t\n\r]+ {
	yyin=fopen(yytext, "r"); // open the new file
	if (yyin == NULL){
		BEGIN(0);
		sprintf(err_array, "file does not exist. %s included", yytext);
		parse_error(err_array);
	}
	else{
		// create a new file buffer and make it current.
		// flex keeps an internal stack of file buffers.
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
		BEGIN(0);
	}
}

<INITIAL><<EOF>> {
	// if this file was an include, will return to
	// previous point. pops and delete's buffer. see <INCLUDE>
	yypop_buffer_state();
	// if we are at the end of the top-level buffer then we are done.
	if ( !YY_CURRENT_BUFFER ) yyterminate();
}
<STRING,CHAR><<EOF>> {
	printf("ERROR: End of file encountered before closing quotation\n");
}

 /******************************* OPERATORS ***********************************/

\( return T_OPAR;
\) return T_CPAR;
\] return T_CBRK;
\[ return T_OBRK;
\{ return T_OBRC;
\} return T_CBRC;

 /* Unary */
@    return T_DREF;
ref  return T_REF;
\+\+ return T_INC;
\-\- return T_DEC;
not  return T_NOT;
inv  return T_INV; // binary inversion
 /* Multiplicative */
\*   return T_MUL;
mod  return T_MOD;
\/   return T_DIV;
\^   return T_EXP;
\<\< return T_LSHFT;
\>\> return T_RSHFT;
 /* Additive */
\+   return T_PLUS;
\-   return T_MINUS;
&    return T_BAND;
\|   return T_BOR;
\|\| return T_BXOR;
 /* Comparative */
=   return T_EQ;
!=  return T_NEQ;
\<  return T_LT;
\>  return T_GT;
\<= return T_LTE;
\>= return T_GTE;
 /* Boolean */
or  return T_OR;
and return T_AND;
 /* Assignments */
:=    return T_ASS;
:\<   return T_LSH_A;
:\>   return T_RSH_A;
:\+   return T_ADD_A;
:\-   return T_SUB_A;
:\*   return T_MUL_A;
:\/   return T_DIV_A;
:%    return T_MOD_A;
:&    return T_AND_A;
:\|   return T_OR_A;
:\|\| return T_XOR_A;

,   return T_LIST;

\-\-\- parse_error("ambiguous construct");
\+\+\+ parse_error("ambiguous construct");


 /**************************** CONTROL KEYWORDS *******************************/

label    return T_LBL;
jump     return T_JMP;
if       return T_IF;
else     return T_ELSE;
switch   return T_SWTCH;
case     return T_CASE;
default  return T_DFLT;
while    return T_WHILE;
do       return T_DO;
break    return T_BRK;
continue return T_CNTN;
try      return T_TRY;
throw    return T_THRW;
catch    return T_CTCH;
for      return T_FOR;
return   return T_RTRN;

 /****************************** DECLARATIONS *********************************/

operator return T_OPR;
sub     return T_SUB;
fun     return T_FUN;

typedef return T_TYPE;

pointer return T_PTR;

byte    return T_8;
byte2   return T_16;
byte4   return T_32;
byte8   return T_64;
word    return T_WORD;
max     return T_MAX;

const  return T_CONST;
static return T_STATIC;
asm    return T_ASM;

 /************************** PRIMARY DATA TYPES *******************************/

{NAME} {
	Bind();
	
	if(!yysymbol) return T_NAME;
	
	switch(yysymbol->type){
	case st_sub:      return T_N_SUB;
	case st_type_def: return T_N_TYPE;
	case st_cust:
	case st_int:
	case st_ref:
	case st_fun: return T_N_FUN;
	
	case st_lit_int:
	case st_lit_str:
	case st_undef:
	case st_NUM:
	default:
		parse_error("scanner.l: NAME: Internal: name binds to invalid type");
	}
}


{HEX} {
	yynumber=0;
	// this is a global from tokens.h used to pass numbers to parser.c
	size_t max=strlen(yytext);
	// yytext is where the matched text from the scanner is stored
	
	if (max-(unsigned)2 > sizeof(umax)*2) // subtracting 2 for the '0x'
		parse_error("constant number too large");
	
	for(int i=2; i<max; i++){ // we start at 2 to skip the '0x'
		if     (yytext[i]>='0' && yytext[i]<='9')
			yynumber= (yynumber<<4) | (yytext[i] - '0');
		
		else if(yytext[i]>='a' && yytext[i]<='f') // lower case
			yynumber= (yynumber<<4) | (yytext[i] - 'a' + 10);
		
		else if(yytext[i]>='A' && yytext[i]<='F') // upper case
			yynumber= (yynumber<<4) | (yytext[i] - 'A' + 10);
		
		else crit_error("Internal: scanner {HEX}");
		// this should be impossible
	}
	
	return T_NUM;
}

{DIGIT}+ yynumber = atoi( yytext ); return T_NUM ; // Decimal number constants


\' { BEGIN(CHAR); }
<CHAR>(\\.|[^'])\' { // an escaped anything \\. or a non-quote [^']
	size_t len = strlen(yytext);
	
	if (len == 2){
		yynumber = yytext[0];
	}
	
	else switch (yytext[1]){
		case 'n':
			yynumber = '\n';
			break;
		case 'r':
			yynumber = '\r';
			break;
		case '\\':
			yynumber = '\\';
			break;
		case '0':
			yynumber = '\0';
			break;
		case 't':
			yynumber = '\t';
			break;
		case '\'':
			yynumber = '\'';
			break;
		default:
			printf(
				"ERROR: Invalid character constant: '%s' on line %d.\n",
				yytext,
				yylineno
			);
	}
	
	BEGIN(0);
	return (T_CHAR);
}


\" { BEGIN(STRING); }
<STRING>(\\.|[^"])*\" {
	int j=0;
	
	for(int i=0; yytext[i] != '"'; i++, j++){
		if (yytext[i] == '\\'){
			i++;
			switch (yytext[i]){
			case 'n':
				yytext[j]='\n';
				break;
			case 'r':
				yytext[j]='\r';
				break;
			case '\\':
				yytext[j]='\\';
				break;
			case '0':
				yytext[j]='\0';
				break;
			case 't':
				yytext[j]='\t';
				break;
			case '"':
				yytext[j]='"';
				break;
			default:
				printf("ERROR: Invalid escape sequence line %d.\n", yylineno);
			}
		}
		else yytext[j]=yytext[i];
	}
	yytext[j]='\0';
	BEGIN(0);
	return(T_STR);
}

 /* This needs to be the last thing */
<*>. {
	printf("Unknown token '%s' on line %d\n", yytext, yylineno);
	parse_error("unknown token");
}


