%{
/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/
%}


%token T_EOF 0 "End of File"
%token T_NL

%token T_NAME T_NUM T_CH T_STR //primary

%token T_INC T_DEC T_REF '@' T_NOT T_INV // Unary
%token '*' '/' T_MOD '^' T_LSHFT T_RSHFT  // Term
%token '+' '-' '&' T_BOR T_BXOR           // Expression
%token '=' T_NEQ '<' '>' T_LTE T_GTE      // Equation
%token T_AND T_OR
%token T_ASS T_LSH_A T_RSH_A T_ADD_A T_SUB_A T_MUL_A T_DIV_A T_MOD_A T_AND_A
%token T_OR_A  T_XOR_A


%token T_8 T_16 T_32 T_64 T_WORD T_MAX
%token T_TYPE T_TO T_PTR
%token T_SUB T_FUN T_OPR T_END T_ASM T_RTRNS
%token T_STATIC T_CONST

%token T_LBL T_JMP T_BRK T_CNTN T_RTRN T_IF T_ELSE T_WHILE T_DO T_FOR T_SWTCH

%start Decl_list

%%


/******************************************************************************/
//                             PARSE EXPRESSIONS
/******************************************************************************/


Call_fun: T_NAME '[' ']'
	;

Primary
	: '(' Assignment ')'
	| T_NUM
	| T_CH
	| T_NAME
	| Call_fun
	| T_STR
	;

Unary
	: Primary
	| T_REF Primary
	| '@'   Unary
	| T_NOT Unary
	| T_INV Unary
	;

Postfix
	: Unary
	;

Term
	: Postfix
	| Term '*'     Postfix
	| Term '/'     Postfix
	| Term T_MOD   Postfix
	| Term '^'     Postfix
	| Term T_LSHFT Postfix
	| Term T_RSHFT Postfix
	;

Expression
	: Term
	| Expression '+'    Term
	| Expression '-'    Term
	| Expression '&'    Term
	| Expression T_BOR  Term
	| Expression T_BXOR Term
	;

Equation
	: Expression
	| Equation '='   Expression
	| Equation T_NEQ Expression
	| Equation '<'   Expression
	| Equation '>'   Expression
	| Equation T_LTE Expression
	| Equation T_GTE Expression
	;

Boolean
	: Equation
	| Boolean T_AND Equation
	| Boolean T_OR  Equation
	;

Assignment_op
	: T_ASS
	| T_LSH_A
	| T_RSH_A
	| T_ADD_A
	| T_SUB_A
	| T_MUL_A
	| T_DIV_A
	| T_MOD_A
	| T_AND_A
	| T_OR_A 
	| T_XOR_A
	;


Assignment
	: Boolean
	| Postfix Assignment_op Assignment
	;


/******************************************************************************/
//                            PARSE DECLARATIONS
/******************************************************************************/


Operator
	: T_INC
	| T_DEC
	| T_REF
	| '@'
	| T_NOT
	| T_INV
	| '*'
	| '/'
	| T_MOD
	| '^'
	| T_LSHFT
	| T_RSHFT
	| '+'
	| '-'
	| '&'
	| T_BOR
	| T_BXOR
	| '='
	| T_NEQ
	| '<'
	| '>'
	| T_LTE
	| T_GTE
	| T_AND
	| T_OR
	| T_ASS
	| T_LSH_A
	| T_RSH_A
	| T_ADD_A
	| T_SUB_A
	| T_MUL_A
	| T_DIV_A
	| T_MOD_A
	| T_AND_A
	| T_OR_A
	| T_XOR_A
	;

Size_specifier
	: T_8
	| T_16
	| T_32
	| T_64
	| T_WORD
	| T_MAX
	;

Storage_class
	: T_STATIC	// Static
				// extern
	;

Type_qualifier
	: T_CONST	// Constant
				// Volatile
	;

Qualifier_list
	: %empty
	| Storage_class
	| Type_qualifier
	| Storage_class Type_qualifier
	| Type_qualifier Storage_class
	;

Initializer
	: T_NAME
	| T_NAME '=' Boolean
	;

Initializer_list
	: Initializer
	| Initializer_list ',' Initializer

Word_specifier: Size_specifier Qualifier_list
	;

Custom_specifier: T_NAME Qualifier_list
	;

Pointer_specifier: T_PTR Qualifier_list T_TO Type_specifier
	;

Type_specifier
	: Word_specifier
	| Pointer_specifier
	| Custom_specifier
	;

Storage_decl: Type_specifier Initializer_list
	;

Parameter_mode
	: T_IN
	| T_OUT
	| T_BI
	;

Parameter
	: Parameter_mode Decl_Symbol_list
	| Parameter_mode Proc_decl
	;

Parameter_list
	: Parameter
	| Parameter_list ';' Parameter
	;

Proc_decl
	: T_FUN T_NAME '[' Parameter_list ']' T_RTRNS Type_specifier
	| T_SUB T_NAME '[' Parameter_list ']'
	;

//Assembler_blk: %empty
//	;

//Op_def: %empty
//| T_OPR       Operator     '[' ']'          T_RTRNS Type_specifier
//	| T_OPR T_ASM Operator '[' ']'              T_RTRNS Type_specifier
//	;

//Proc_def
//	: Proc_decl Decl_list Statement T_END T_NAME T_NL
//	;


Type_decl: T_TYPE T_NAME Decl_list
	;

Declaration
	: Type_decl
//	| Sub_def
//	| Fun_def
//	| Operator_Decl
	| Storage_decl T_NL
	;

Decl_list
	: Declaration
	| Decl_list Declaration
	;


/******************************************************************************/
//                             PARSE STATEMENTS
/******************************************************************************/


Call_sub: T_NAME
	;

Label: T_LBL T_NAME T_NL
	;

Jump: T_JMP T_NAME T_NL
	;

Break: T_BRK T_NL
	;

Continue: T_CNTN T_NL
	;

Return
	: T_RTRN T_NL
	| T_RTRN Boolean T_NL
	;

If
	: T_IF Boolean Statement
	| T_IF Boolean Statement T_ELSE Statement
	;

While: T_WHILE Boolean Statement
	;

Do: T_DO Statement T_WHILE Boolean T_NL
	;

For: T_FOR
	;

Switch: T_SWTCH Boolean
	;

Statement_block
	: Statement
	| Statement_block Statement
	;

Statement
	: T_NL // empty statement
	| T_NL Statement_block
	| Label
	| Jump
	| Break
	| Continue
	| Return
	| If
	| While
	| Do
	| For
	| Switch
	| Call_sub
	| Assignment T_NL
	;


/******************************************************************************/
//                                     PARSE
/******************************************************************************/


Statement_list
	: Statement
	| Statement_list Statement
	;

Unit
	: Decl_list T_EOF
	| Statement_list T_EOF
	| Decl_list Statement_list T_EOF
	;


