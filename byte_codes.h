#ifndef _BYTE_CODES_H
#define _BYTE_CODES_H

/*
 *	OMNI INTERMEDIATE OP CODES
 */

char * intermediate_op_code_index[50] = {
	"I_NUL", "I_ASS", "I_REF", "I_DREF", "I_NEG", "I_NOT" , "I_INV" ,"I_MUL"  ,
	"I_DIV", "I_MOD", "I_EXP", "I_LSH" , "I_RSH", "I_ADD" , "I_SUB" , "I_BAND",
	"I_BOR", "I_XOR", "I_EQ" , "I_NEQ" , "I_LT" , "I_GT"  , "I_LTE" , "I_GTE" ,
	"I_AND", "I_OR" , "I_JMP", "I_JZ"  , "I_BLK", "I_EBLK", "I_CALL", "I_RTRN"
};




//#define I_NUL  (byte_code)0x00

//// Unary OPS (6)
//#define I_ASS  (byte_code)0x01
//#define I_REF  (byte_code)0x02
//#define I_DREF (byte_code)0x03
//#define I_NEG  (byte_code)0x04
//#define I_NOT  (byte_code)0x05
//#define I_INV  (byte_code)0x06

//// Binary OPS (19)
//#define I_MUL  (byte_code)0x10
//#define I_DIV  (byte_code)0x11
//#define I_MOD  (byte_code)0x12
//#define I_EXP  (byte_code)0x13
//#define I_LSH  (byte_code)0x14
//#define I_RSH  (byte_code)0x15

//#define I_ADD  (byte_code)0x16
//#define I_SUB  (byte_code)0x17
//#define I_BAND (byte_code)0x18
//#define I_BOR  (byte_code)0x19
//#define I_XOR  (byte_code)0x1a

//#define I_EQ   (byte_code)0x1b
//#define I_NEQ  (byte_code)0x1c
//#define I_LT   (byte_code)0x1d
//#define I_GT   (byte_code)0x1e
//#define I_LTE  (byte_code)0x1f
//#define I_GTE  (byte_code)0x20

//#define I_AND  (byte_code)0x21
//#define I_OR   (byte_code)0x22

//// Flow Control (6)
//#define I_JMP  (byte_code)0x30
//#define I_JZ   (byte_code)0x31
//#define I_BLK  (byte_code)0x32
//#define I_EBLK (byte_code)0x33
//#define I_CALL (byte_code)0x34
//#define I_RTRN (byte_code)0x35


#endif // _BYTE_CODES_H
