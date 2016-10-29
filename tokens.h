#ifndef _TOKENS_H
#define _TOKENS_H


/******************************************************************************/
//                       Primary data and formatting
/******************************************************************************/


#define T_EOF  (token_t)0x0000 // yyterminate returns a 0 automatically
#define T_NL   (token_t)0x0001 // sets block_lvl
#define T_NUM  (token_t)0x0002 // sets yynumber
#define T_NAME (token_t)0x0003 // sets yytext
#define T_STR  (token_t)0x0004 // sets yytext
//#define T_CHAR (token_t)0x0005 // sets yytext
// char will eventually be a variable width encoding, but for now it is just byte


/******************************************************************************/
//                                Operators
/******************************************************************************/


// order of operations
#define T_OPAR  (token_t)0x0010
#define T_CPAR  (token_t)0x0011
#define T_OBRC  (token_t)0x0012
#define T_CBRC  (token_t)0x0013
#define T_OBRK  (token_t)0x0014
#define T_CBRK  (token_t)0x0015

// unary
#define T_DEC   (token_t)0x0016
#define T_INC   (token_t)0x0017
#define T_REF   (token_t)0x0018
#define T_DREF  (token_t)0x0019
#define T_NOT   (token_t)0x001a
#define T_INV   (token_t)0x001b

// basic
#define T_PLUS  (token_t)0x001c
#define T_MINUS (token_t)0x001d
#define T_BAND  (token_t)0x001e
#define T_BOR   (token_t)0x001f
#define T_BXOR  (token_t)0x0020

// multiplicative
#define T_MUL   (token_t)0x0021
#define T_MOD   (token_t)0x0022
#define T_DIV   (token_t)0x0023
#define T_EXP   (token_t)0x0024
#define T_LSHFT (token_t)0x0025
#define T_RSHFT (token_t)0x0026

// comparative
#define T_EQ    (token_t)0x0027
#define T_NEQ   (token_t)0x0028
#define T_LT    (token_t)0x0029
#define T_GT    (token_t)0x002a
#define T_LTE   (token_t)0x002b
#define T_GTE   (token_t)0x002c

// boolean
#define T_AND   (token_t)0x002d
#define T_OR    (token_t)0x002e

// assignment
#define T_ASS   (token_t)0x002f // assignments in function calls are pipes
#define T_LSH_A (token_t)0x0030
#define T_RSH_A (token_t)0x0031
#define T_ADD_A (token_t)0x0032
#define T_SUB_A (token_t)0x0033
#define T_MUL_A (token_t)0x0034
#define T_DIV_A (token_t)0x0035
#define T_MOD_A (token_t)0x0036
#define T_AND_A (token_t)0x0037
#define T_OR_A  (token_t)0x0038
#define T_XOR_A (token_t)0x0039

#define T_LIST  (token_t)0x003a


/******************************************************************************/
//                            Control statements
/******************************************************************************/


#define T_LBL   (token_t)0x0040
#define T_JMP   (token_t)0x0041
#define T_IF    (token_t)0x0042
#define T_ELSE  (token_t)0x0043
#define T_SWTCH (token_t)0x0044
#define T_DFLT  (token_t)0x0045
#define T_WHILE (token_t)0x0046
#define T_DO    (token_t)0x0047
#define T_BRK   (token_t)0x0048
#define T_CNTN  (token_t)0x0049
#define T_TRY   (token_t)0x004a
#define T_THRW  (token_t)0x004b
#define T_CTCH  (token_t)0x004c
#define T_FOR   (token_t)0x004d
#define T_RTRN  (token_t)0x004e
#define T_END   (token_t)0x004f // End of Subroutine or function definition


/******************************************************************************/
//                               Declarations
/******************************************************************************/


#define T_OPR  (token_t)0x0060 // Operator Declaration

// Word declarations
#define T_8    (token_t)0x0061
#define T_16   (token_t)0x0062
#define T_24   (token_t)0x0063
#define T_32   (token_t)0x0064
#define T_64   (token_t)0x0065
#define T_WORD (token_t)0x0066
#define T_MAX  (token_t)0x0067

// pointer declaration
#define T_PTR (token_t)0x0068
#define T_TO  (token_t)0x0069

#define T_SUB  (token_t)0x006a // Subroutine
#define T_FUN  (token_t)0x006b // Function
#define T_TYPE (token_t)0x006c // Type Declaration

// Qualifers
#define T_CONST  (token_t)0x0070
#define T_STATIC (token_t)0x0071
#define T_ASM    (token_t)0x0072 // Assembler Routine

/******************************************************************************/
//                                Registers
/******************************************************************************/


#define T_R0  (token_t)0x0080
#define T_R1  (token_t)0x0081
#define T_R2  (token_t)0x0082
#define T_R3  (token_t)0x0083
#define T_R4  (token_t)0x0084
#define T_R5  (token_t)0x0085
#define T_R6  (token_t)0x0086
#define T_R7  (token_t)0x0087
#define T_R8  (token_t)0x0088
#define T_R9  (token_t)0x0089
#define T_R10 (token_t)0x008a
#define T_R11 (token_t)0x008b
#define T_R12 (token_t)0x008c
#define T_R13 (token_t)0x008d
#define T_R14 (token_t)0x008e
#define T_R15 (token_t)0x008f


/******************************************************************************/
//                       Machine Independent Assembler
/******************************************************************************/


#endif // _TOKENS_H

