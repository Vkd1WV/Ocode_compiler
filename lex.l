%{

/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#define _LEX_L // Used in globals.h to eliminate spurrious warnings

#include "lex.h"

%}


/* prevents flex from generating the default rule
 do not expect to call yywrap at the end of the file
 prevents the unput function
 */
%option nodefault noyywrap nounput noinput

/* keep track of the line number in yylineno
 turn on warnings */
%option yylineno warn

%x INCLUDE
%x STRING
%x CHAR

DIGIT  [0-9]
HEX    0x({DIGIT}|[a-f])+
LETTER [A-Za-z_]
NAME   {LETTER}({DIGIT}|{LETTER})*
WHTSPC [ \t\v]

NEWL   ({WHTSPC})*(\r|\r\n|\n)


/******************************************************************************/
/*                                  RULES*/
/******************************************************************************/
%%


 /* These must eat all tabs at the begining of lines that shouldn't trigger indentation */
({WHTSPC})*#.*{NEWL} { tabs_this_ln=0;              } // comments
^({NEWL})            { tabs_this_ln=0;              } // ignore empty lines
({WHTSPC})*\\(\r|\r\n|\n) {} // ignore escaped newlines
{NEWL}               { tabs_this_ln=0; return T_NL; } // report the rest

^[ *\t+] {
	for(unsigned int i=0; i < yyleng; i++)
		if (yytext[i] == '\t') tabs_this_ln++;
}

({WHTSPC})+  {} // ignore all other whitespace



 /* handle file inclusions */
^include{WHTSPC} { BEGIN(INCLUDE); } // file include
<INCLUDE>[^ \t\n\r]+ {
	yyin=fopen(yytext, "r"); // open the new file
	if (yyin == NULL){
		BEGIN(INITIAL);
		scan_error("File does not exist");
		return T_SERR;
	}
	else{
		// create a new file buffer and make it current.
		// flex keeps an internal stack of file buffers.
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
		BEGIN(INITIAL);
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

,   return T_COMA;

\-\-\- scan_error("ambiguous construct"); return T_SERR;
\+\+\+ scan_error("ambiguous construct"); return T_SERR;


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

{NAME} return T_NAME;


{HEX} {
//	yynumber=0; // this is a global from scanner.h
//	size_t max;
//	
//	max=strlen(yytext);
//	if (max-(unsigned)2 > sizeof(unsigned long long)*2) // subtracting 2 for the '0x'
//		return scan_error("constant number too large");
//	
//	for(int i=2; i<max; i++){ // we start at 2 to skip the '0x'
//		if     (yytext[i]>='0' && yytext[i]<='9')
//			yynumber= (yynumber<<4) | (unsigned char)(yytext[i] - '0');
//		
//		else if(yytext[i]>='a' && yytext[i]<='f') // lower case
//			yynumber= (yynumber<<4) | (unsigned char)(yytext[i] - 'a' + 10);
//		
//		else if(yytext[i]>='A' && yytext[i]<='F') // upper case
//			yynumber= (yynumber<<4) | (unsigned char)(yytext[i] - 'A' + 10);
//		
//		else scan_error("Internal: {HEX}");
//		// this should be impossible
//	}
	
	return T_NUM;
}

    /* Decimal number constants */
{DIGIT}+ /*yynumber = (unsigned)atoi( yytext );*/ return T_NUM ;


\' { BEGIN(CHAR); }
<CHAR>(\\.|[^'])\' { // an escaped anything \\. or a non-quote [^']
//	size_t len = strlen(yytext);
//	
//	if (len == 2){
//		yynumber = (unsigned char)yytext[0];
//	}
//	
//	else
//		switch (yytext[1]){
//		case 'n':
//			yynumber = '\n';
//			break;
//		case 'r':
//			yynumber = '\r';
//			break;
//		case '\\':
//			yynumber = '\\';
//			break;
//		case '0':
//			yynumber = '\0';
//			break;
//		case 't':
//			yynumber = '\t';
//			break;
//		case '\'':
//			yynumber = '\'';
//			break;
//		default:
//			printf(
//				"ERROR: Invalid character constant: '%s' on line %d.\n",
//				yytext,
//				yylineno
//			);
//	}
	
	BEGIN(INITIAL);
	return T_CHAR;
}


\" { BEGIN(STRING); }
<STRING>(\\.|[^"])*\" {
//	int j=0;
//	
//	for(int i=0; yytext[i] != '"'; i++, j++){
//		if (yytext[i] == '\\'){
//			i++;
//			switch (yytext[i]){
//			case 'n':
//				yytext[j]='\n';
//				break;
//			case 'r':
//				yytext[j]='\r';
//				break;
//			case '\\':
//				yytext[j]='\\';
//				break;
//			case '0':
//				yytext[j]='\0';
//				break;
//			case 't':
//				yytext[j]='\t';
//				break;
//			case '"':
//				yytext[j]='"';
//				break;
//			default:
//				printf("ERROR: Invalid escape sequence line %d.\n", yylineno);
//			}
//		}
//		else yytext[j]=yytext[i];
//	}
//	yytext[j]='\0';
	BEGIN(INITIAL);
	return T_STR;
}

 /* This needs to be the last thing */
<*>[\n.] scan_error("unknown token");


