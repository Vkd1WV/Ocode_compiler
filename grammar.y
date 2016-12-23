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
%token T_NL T_IND T_OUTD

%token T_NAME T_N_FUN T_N_SUB T_N_TYPE T_N_STRG T_N_CLSS

%token T_NUM T_CHAR T_STR //primary

%token T_INC T_DEC T_REF '@' T_NOT T_INV // Unary
%token '*' '/' T_MOD '^' T_LSHFT T_RSHFT  // Term
%token '+' '-' '&' T_BOR T_BXOR           // Expression
%token '=' T_NEQ '<' '>' T_LTE T_GTE      // Equation
%token T_AND T_OR
%token T_ASS T_LSH_A T_RSH_A T_ADD_A T_SUB_A T_MUL_A T_DIV_A T_MOD_A T_AND_A
%token T_OR_A  T_XOR_A


%token T_8 T_16 T_32 T_64 T_WORD T_MAX
%token T_TYPE T_TO T_PTR
%token T_SUB T_FUN T_OPR T_END T_ASM
%token T_STATIC T_EXTRN T_CONST T_VOL T_IN T_OUT T_BI T_VOID

%token T_LBL T_JMP T_BRK T_CNTN T_RTRN 
%token T_IF T_ELSE T_WHILE T_DO T_FOR T_SWTCH

%start Unit

%%


/******************************************************************************/
//                                EXPRESSIONS
/******************************************************************************/


argument_expression_list
	: Assignment
	| argument_expression_list ',' Assignment
	;

Call_fun
	: T_N_FUN '[' argument_expression_list ']'
	| T_N_FUN '[' ']'
	;

Primary
	: '(' Assignment ')'
	| T_NUM
	| T_CHAR
	| Call_fun
	| T_STR
	| T_N_STRG
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
	| T_NAME T_ASS Assignment // implicit declaration
	;

Constant_Expression: Boolean
	;


/******************************************************************************/
//                               DECLARATIONS
/******************************************************************************/


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
	| T_EXTRN	// extern
	;

Type_qualifier
	: T_CONST	// Constant
	| T_VOL		// Volatile
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
	| T_NAME '=' Constant_Expression
	;

Initializer_list
	: Initializer
	| Initializer_list ',' Initializer
	;

Word_specifier: Size_specifier Qualifier_list
	;

Custom_specifier: T_N_TYPE Qualifier_list
	;

Pointer_specifier: T_PTR Qualifier_list T_TO Type_specifier
	;

Type_specifier
	: Word_specifier
	| Pointer_specifier
	| Custom_specifier
	| T_VOID
	;

Decl_Storage: Type_specifier Initializer_list
	;


//Op_def: %empty
//| T_OPR       Operator     '[' ']'          T_RTRNS Type_specifier
//	| T_OPR T_ASM Operator '[' ']'              T_RTRNS Type_specifier
//	;

//Proc_def
//	: Proc_decl Decl_list Statement T_END T_NAME T_NL
//	;

//Subtype_specifier
//	: T_CLASS
//	| T_UNION
//	| T_REC
//	;


//Decl_Type
//	: T_TYPE T_NAME Subtype_specifier Declaration_block
//	| T_TYPE T_NAME Type_specifier T_NL // alias
//	;

//Declaration
//	: Decl_Storage
//	| Decl_Fun
//	| Decl_Inheiritance
//	;

//Decl_list
//	: Declaration
//	| Decl_list Declaration
//	;

//Declaration_block: T_IND Decl_list T_OUTD
//	;


/******************************************************************************/
//                          PROCEDURE DECLARATIONS
/******************************************************************************/


Assembler_blk: %empty
	;

Procedure_Body
	:       Local_Statement T_END
	| T_ASM Assembler_blk   T_END
	;

/********************************* Routine ************************************/

// has unstructured parameters and preset io streams

/******************************* Subroutine ***********************************/



// Unrestricted call
//Decl_Sub
//	: T_SUB T_NAME '[' Parameter_list ']' Procedure_Body
//	;

/******************************** Function ************************************/

//Decl_Fun_Parameter
//	: Type_specifier Initializer
//	;

//Fun_Parameter_list
//	: Decl_Fun_Parameter
//	| Fun_Parameter_list ',' Decl_Fun_Parameter
//	;

Parameter_mode
	: T_IN
	| T_OUT
	| T_BI
	;

Decl_Parameter
	: Parameter_mode Type_specifier Initializer
	;

Parameter_list
	: Decl_Parameter
	| Parameter_list ',' Decl_Parameter
	;

Decl_Fun
	: T_FUN Type_specifier T_NAME '[' Parameter_list ']' Procedure_Body
	;

/******************************** Operator ************************************/

//Operator
//	: T_INC
//	| T_DEC
//	| T_REF
//	| '@'
//	| T_NOT
//	| T_INV
//	| '*'
//	| '/'
//	| T_MOD
//	| '^'
//	| T_LSHFT
//	| T_RSHFT
//	| '+'
//	| '-'
//	| '&'
//	| T_BOR
//	| T_BXOR
//	| '='
//	| T_NEQ
//	| '<'
//	| '>'
//	| T_LTE
//	| T_GTE
//	| T_AND
//	| T_OR
//	| T_ASS
//	| T_LSH_A
//	| T_RSH_A
//	| T_ADD_A
//	| T_SUB_A
//	| T_MUL_A
//	| T_DIV_A
//	| T_MOD_A
//	| T_AND_A
//	| T_OR_A
//	| T_XOR_A
//	;

//Decl_Operator: T_OPR Operator '[' Decl_Fun_Parameter ']' Procedure_Body
//	;


/******************************************************************************/
//                            CONTROL STATEMENTS
/******************************************************************************/


Call_sub: T_N_SUB
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
	| T_RTRN Assignment T_NL
	;

If
	: T_IF Assignment Local_Statement
	| T_IF Assignment Local_Statement T_ELSE Local_Statement
	;

While: T_WHILE Assignment Local_Statement
	;

Do: T_DO Local_Statement T_WHILE Assignment T_NL
	;

For: T_FOR
	;

Switch: T_SWTCH Boolean
	;


/******************************************************************************/
//                                  STATEMENTS
/******************************************************************************/


// No empty statements or blocks
Global_Statement
	: Decl_Storage
	| Decl_Fun
	//| Decl_Sub
	//| Decl_Type
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

// No function declarations
Local_Statement
	: T_NL // empty statement
	| T_NL Statement_block
	| Decl_Storage
	//| Decl_Type
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

Global_Statement_list
	: Global_Statement
	| Global_Statement_list Global_Statement
	;

Local_Statement_list
	: Local_Statement
	| Local_Statement_list Local_Statement
	;


Statement_block: T_IND Local_Statement_list T_OUTD
	;

Unit: Global_Statement_list T_EOF
	;


