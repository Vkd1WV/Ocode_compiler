

#include "scanner.h"
#include <stdlib.h>

int main(int argc, char** argv){
	token_t atoken;
	
	if(argc > 1){
		yyin = fopen(argv[1], "r");
		if(!yyin){
			puts("no such file");
			return EXIT_FAILURE;
		}
	}
	
	do{
		atoken = yylex();
		
		printf("%u: %s  \t\"%s\"\n",
			tabs_this_ln,
			token_dex[atoken],
			token != T_NL? yytext : ""
		);
		
	} while(atoken);
	
	return EXIT_SUCCESS;
}
