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

#[^\n]* {} // comments

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
\+\+ return T_INC;
\-\- return T_DEC;
not  return T_NOT;
inv  return T_INV; // binary inversion
 /* Multiplicative */
\*   return T_TIMES;
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
:   return T_ASS;

\-\-\- error("ambiguous construct");
\+\+\+ error("ambiguous construct");


 /**************************** CONTROL KEYWORDS *******************************/

label    return T_LBL;
jump     return T_JMP;
if       return T_IF;
else     return T_ELSE;
switch   return T_SWTCH;
default  return T_DFLT;
while    return T_WHILE;
do       return T_DO;
break    return T_BRK;
continue return T_CNTN;
try      return T_TRY;
throw    return T_THRW;
catch    return T_CTCH;
for      return T_FOR;

 /***************************** DATA KEYWORDS *********************************/

byte  return T_8;
word  return T_16;
dword return T_32;
qword return T_64;

const  return T_CONST;
static return T_STATIC;

sub     return T_SUB;
end     return T_END;
typedef return T_TYPE;

 /******************************* REGISTERS ***********************************/

R0  return T_R0; // A
R1  return T_R1; // C
R2  return T_R2; // D
R3  return T_R3; // B
R4  return T_R4; // SP
R5  return T_R5; // BP
R6  return T_R6; // SI
R7  return T_R7; // DI

R8  {
	if (arch == x86 && x86_mode != Long)
		error("R8 is only availible in Long mode");
	return T_R8;
}
R9  {
	if (arch == x86 && x86_mode != Long)
		error("R9 is only availible in Long mode");
	return T_R9;
}
R10 {
	if (arch == x86 && x86_mode != Long)
		error("R10 is only availible in Long mode");
	return T_R10;
}
R11 {
	if (arch == x86 && x86_mode != Long)
		error("R11 is only availible in Long mode");
	return T_R11;
}
R12 {
	if (arch == x86 && x86_mode != Long)
		error("R12 is only availible in Long mode");
	return T_R12;
}
R13 {
	if (arch == x86 && x86_mode != Long)
		error("R13 is only availible in Long mode");
	return T_R13;
}
R14 {
	if (arch == x86 && x86_mode != Long)
		error("R14 is only availible in Long mode");
	return T_R14;
}
R15 {
	if (arch == x86 && x86_mode != Long)
		error("R15 is only availible in Long mode");
	return T_R15;
}

A  {
	if (arch == x86) return T_R0;
	printf("You really shouldn't be using register name A on line %d\n", yylineno);
	REJECT;
}
B  {
	if (arch == x86) return T_R4;
	printf("You really shouldn't be using register name B on line %d\n", yylineno);
	REJECT;
}
C  {
	if (arch == x86) return T_R1;
	printf("You really shouldn't be using register name C on line %d\n", yylineno);
	REJECT;
}
D  {
	if (arch == x86) return T_R2;
	printf("You really shouldn't be using register name D on line %d\n", yylineno);
	REJECT;
}
SI {
	if (arch == x86) return T_R7;
	printf("You really shouldn't be using register name SI on line %d\n", yylineno);
	REJECT;
}
DI {
	if (arch == x86) return T_R8;
	printf("You really shouldn't be using register name DI on line %d\n", yylineno);
	REJECT;
}
BP {
	if (arch == x86) return T_R6;
	printf("You really shouldn't be using register name BP on line %d\n", yylineno);
	REJECT;
}
SP {
	if (arch == x86) return T_R5;
	printf("You really shouldn't be using register name SP on line %d\n", yylineno);
	REJECT;
}

 /************************** PRIMARY DATA TYPES *******************************/

{HEX} {
	yynumber=0;
	// this is a global from tokens.h used to pass numbers to parser.c
	int max=strlen(yytext);
	// yytext is where the matched text from the scanner is stored
	
	if (max-(unsigned)2 > sizeof(umax)*2) // subtracting 2 for the '0x'
		error("constant number too large");
	
	for(int i=2; i<max; i++){ // we start at 2 to skip the '0x'
		if     (yytext[i]>='0' && yytext[i]<='9')
			yynumber= (yynumber<<4) | (yytext[i] - '0');
		
		else if(yytext[i]>='a' && yytext[i]<='f') // lower case
			yynumber= (yynumber<<4) | (yytext[i] - 'a' + 10);
		
		else if(yytext[i]>='A' && yytext[i]<='F') // upper case
			yynumber= (yynumber<<4) | (yytext[i] - 'A' + 10);
		
		else error("internal compiler error in scanner {HEX}");
		// this should be impossible
	}
	
	return T_NUM;
}

{DIGIT}+ yynumber = atoi( yytext ); return T_NUM ; // Decimal number constants
{NAME}   {
	if(strlen(yytext)-1 > NAME_MAX) error("name is too long");
	return T_NAME;
}



\' { BEGIN(CHAR); }
<CHAR>(\\.|[^'])\' { // an escaped anything \\. or a non-quote [^']
	int len = strlen(yytext);
	
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
	return (T_NUM);
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
	error("unknown token");
}


