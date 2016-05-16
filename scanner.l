%{
/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This is the Omega token scanner
//
/******************************************************************************/

#include "compiler.h"
#define YY_DECL token_t yylex(void)
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


^({NEWL}) {} // ignore empty lines

{NEWL}   block_lvl=0; return T_NL;
^[ *\t+] { // count leading tabs and pass in block_lvl
	for(int i=0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\t') block_lvl++;
}

{WHTSPC} {} // ignore whitespace for now

 /* handle file inclusions */
include{WHTSPC} { BEGIN(INCLUDE); } // file include
<INCLUDE>[^ \t\n\r]+ {
	yyin=fopen(yytext, "r"); // open the new file
	if (yyin == NULL){
		printf(
			"ERROR: file %s, included on line %d does not exist.\n",
			yytext,
			yylineno
		);
		exit(EXIT_FAILURE);
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
\$   return T_REF;
@    return T_DREF;
\+\+ return T_INC;
\-\- return T_DEC;
!    return T_NOT;
inv  return T_INV; // binary inversion
 /* Additive */
\+   return T_PLUS;
\-   return T_MINUS;
 /* Multiplicative */
\*   return T_TIMES;
\%   return T_MODUL;
\/   return T_DIV;
\<\< return T_LSHFT;
\>\> return T_RSHFT;
 /* Binary */
and  return T_BAND;
or   return T_BOR;
xor  return T_BXOR;
 /* Comparative */
=   return T_EQ;
!=  return T_NEQ;
\<  return T_LT;
\>  return T_GT;
=\< return T_LTE;
=\> return T_GTE;

 /* Boolean */
\|  return T_OR;
&   return T_AND;

:=  return T_ASS;


 /**************************** CONTROL KEYWORDS *******************************/

label    return T_LBL;
jmp      return T_JMP;
if       return T_IF;
else     return T_ELSE;
switch   return T_SWTCH;
default  return T_DFLT;
while    return T_WHILE;
do       return T_DO;
break    return T_BRK;
continue return T_CNTN;

 /***************************** DATA KEYWORDS *********************************/

int8  return T_INT8;
int16 return T_INT16;
int32 return T_INT32;
int64 return T_INT64;
int   return T_INT;
intm  return T_INTM;

uint8  return T_UINT8;
uint16 return T_UINT16;
uint32 return T_UINT32;
uint64 return T_UINT64;
uint   return T_UINT;
uintm  return T_UINTM;

char   return T_CH;

const  return T_CONST;
static return T_STATIC;

 /************************** PRIMARY DATA TYPES *******************************/

{HEX} {
	yynumber=0;
	// this is a global from tokens.h used to pass numbers to parser.c
	int max=strlen(yytext);
	// yytext is where the matched text from the scanner is stored
	
	if (max-(unsigned)2 > sizeof(umax)*2) // subtracting 2 for the '0x'
		Abort("constant number too large");
	
	for(int i=2; i<max; i++){ // we start at 2 to skip the '0x'
		if     (yytext[i]>='0' && yytext[i]<='9')
			yynumber= (yynumber<<4) | (yytext[i] - '0');
		
		else if(yytext[i]>='a' && yytext[i]<='f') // lower case
			yynumber= (yynumber<<4) | (yytext[i] - 'a' + 10);
		
		else if(yytext[i]>='A' && yytext[i]<='F') // upper case
			yynumber= (yynumber<<4) | (yytext[i] - 'A' + 10);
		
		else Abort("internal compiler error in scanner {HEX}");
		// this should be impossible
	}
	
	return T_NUM;
}

{DIGIT}+ yynumber = atoi( yytext ); return T_NUM ; // Decimal number constants
{NAME}   return T_NAME;



\' { BEGIN(CHAR); }
<CHAR>(\\.|[^'])\' { // an escaped anything \\. or a non-quote [^']
	int len = strlen(yytext);
	BEGIN(0);
	
	yytext[len-1] = '\0'; // get rid of the trailing '
	
	if (len == 2) return (T_CHAR);
	
	switch (yytext[1]){
	case 'n':
		yytext[0]='\n';
		yytext[1]='\0';
		break;
	case 'r':
		yytext[0]='\r';
		yytext[1]='\0';
		break;
	case '\\':
		yytext[0]='\\';
		yytext[1]='\0';
		break;
	case '0':
		yytext[0]='\0';
		yytext[1]='\0';
		break;
	case 't':
		yytext[0]='\t';
		yytext[1]='\0';
		break;
	case '\'':
		yytext[0]='\'';
		yytext[1]='\0';
		break;
	default:
		printf(
			"ERROR: Invalid character constant: '%s' on line %d.\n",
			yytext,
			yylineno
		);
	}
	return(T_CHAR);
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
	Abort("unknown token");
}


