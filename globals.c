#define _GLOBALS_C

#include "compiler.h"

const char * byte_code_dex[NUM_I_CODES] = {
	"I_NUL", "I_ASS", "I_REF", "I_DREF", "I_NEG", "I_NOT" , "I_INV" ,"I_MUL"  ,
	"I_DIV", "I_MOD", "I_EXP", "I_LSH" , "I_RSH", "I_ADD" , "I_SUB" , "I_BAND",
	"I_BOR", "I_XOR", "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  , "I_LTE" , "I_GTE" ,
	"I_AND", "I_OR" , "I_JMP", "I_JZ"  , "I_BLK", "I_EBLK", "I_CALL", "I_RTRN"
};

const char * default_out  = "out"  ;
const char * default_dbg  = ".dbg" ;
const char * default_asm  = ".asm" ;
const char * default_pexe = ".pexe";
