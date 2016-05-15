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
#define T_CHAR (token_t)0x0005 // sets yytext


/******************************************************************************/
//                                Operators
/******************************************************************************/


// order of operations
#define T_OPAR  (token_t)0x0010
#define T_CPAR  (token_t)0x0011
// unary
#define T_DEC   (token_t)0x0012
#define T_INC   (token_t)0x0013
#define T_REF   (token_t)0x0014
#define T_DREF  (token_t)0x0015
#define T_NOT   (token_t)0x0016
#define T_INV   (token_t)0x0017
// multiplicative
#define T_TIMES (token_t)0x0018
#define T_MODUL (token_t)0x0019
#define T_DIV   (token_t)0x001a
#define T_LSHFT (token_t)0x001b
#define T_RSHFT (token_t)0x001c
// additive
#define T_PLUS  (token_t)0x001d
#define T_MINUS (token_t)0x001e
#define T_BAND  (token_t)0x001f
#define T_BOR   (token_t)0x0020
#define T_BXOR  (token_t)0x0021
// comparative
#define T_EQ    (token_t)0x0022
#define T_NEQ   (token_t)0x0023
#define T_LT    (token_t)0x0024
#define T_GT    (token_t)0x0025
#define T_LTE   (token_t)0x0026
#define T_GTE   (token_t)0x0027
// boolean
#define T_AND   (token_t)0x0028
#define T_OR    (token_t)0x0029
// assignment
#define T_ASS  (token_t)0x002a


/******************************************************************************/
//                            Control statements
/******************************************************************************/


#define T_LBL   (token_t)0x0030
#define T_JMP   (token_t)0x0031
#define T_IF    (token_t)0x0032
#define T_ELSE  (token_t)0x0033
#define T_SWTCH (token_t)0x0034
#define T_DFLT  (token_t)0x0035
#define T_WHILE (token_t)0x0036
#define T_DO    (token_t)0x0037
#define T_BRK   (token_t)0x0038
#define T_CNTN  (token_t)0x0039


/******************************************************************************/
//                               Data types
/******************************************************************************/


#define T_INT8  (token_t)0x0040
#define T_INT16 (token_t)0x0041
#define T_INT32 (token_t)0x0042
#define T_INT64 (token_t)0x0043
#define T_INT   (token_t)0x0044
#define T_INTM  (token_t)0x0045

#define T_UINT8  (token_t)0x0046
#define T_UINT16 (token_t)0x0047
#define T_UINT32 (token_t)0x0048
#define T_UINT64 (token_t)0x0049
#define T_UINT   (token_t)0x004a
#define T_UINTM  (token_t)0x004b

#define T_CH     (token_t)0x004c

#define T_CONST  (token_t)0x004d
#define T_STATIC (token_t)0x004e

#endif // _TOKENS_H
