#include <stdio.h>
#include <stdlib.h>

#define TMP_ARR_SZE 100
extern int yylineno;
extern uint16_t token;

static inline void error   (const char* message);
static inline void Abort   (const char* message);
static inline void expected(const char* message);

static inline void error(const char* message){
	printf("ERROR: %s, on line %d\n", message, yylineno);
}

static inline void Abort(const char* message){
	error(message);
	exit(EXIT_FAILURE);
}

static inline void expected(const char* thing){
	char temp_array[TMP_ARR_SZE];
	sprintf(temp_array, "Expected %s, found 0x%x", thing, token);
	Abort(temp_array);
}


