%token T_NL T_EOF

%token T_NAME T_NUM T_CH T_STR //primary
%token T_OPAR T_CPAR

%token T_INC T_DEC T_REF T_DREF T_NOT T_INV  // Unary
%token T_MOD T_LSHFT T_RSHFT // Term
%token T_BOR T_BXOR    // Expression
%token T_NEQ T_LT T_GT T_LTE T_GTE      // Equation
%token T_AND T_OR
%token T_ASS T_LSH_A T_RSH_A T_ADD_A T_SUB_A T_MUL_A T_DIV_A T_MOD_A T_AND_A
%token T_OR_A  T_XOR_A


%token T_8 T_16 T_32 T_64 T_WORD T_MAX
%token T_TYPE T_TO T_PTR
%token T_SUB T_FUN T_OPR T_END T_ASM
%token T_STATIC T_CONST

%token T_LBL T_JMP T_BRK T_CNTN T_RTRN T_IF T_ELSE T_WHILE T_DO T_SWTCH

%%


/******************************************************************************/
//                             PARSE EXPRESSIONS
/******************************************************************************/


Call_fun: %empty
	;

Primary
	: '(' Boolean ')'
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
	| Assign
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

Assign: Postfix Assignment_op Boolean
	;

//assignment
//	: Postfix
//	| Postfix assignment_list
//	;

//assignment_list
//	: assignment_op assignment
//	| assignment_list assignment_op assignment
//	;


/******************************************************************************/
//                            PARSE DECLARATIONS
/******************************************************************************/


Size_specifier
	: T_8
	| T_16
	| T_32
	| T_64
	| T_WORD
	| T_MAX
	;

Qualifier_list
	: T_CONST
	| T_STATIC
	;

Initializer
	: T_NAME
	| T_NAME '=' Boolean
	;

Initializer_list
	: Initializer T_NL
	| Initializer_list ',' Initializer T_NL

Specify_word: Size_specifier Qualifier_list
	;

Specify_custom: T_NAME Qualifier_list
	;

Specify_pointer: T_PTR Qualifier_list T_TO Type_specifier
	;

Type_specifier
	: Specify_word
	| Specify_pointer
	| Specify_custom
	;

Decl_Symbol_list: Type_specifier Initializer_list
	;

Parameter_list: %empty
	;

Assembler_blk: %empty
	;

Decl_Operator: %empty
	;

Decl_Sub
	: T_SUB       T_NAME Parameter_list Decl_list Statement T_END T_NAME T_NL
	| T_SUB T_ASM T_NAME Parameter_list Assembler_blk       T_END T_NAME T_NL
	;

Decl_Fun
	: T_FUN       T_NAME '[' Parameter_list ']'
		Decl_list Statement T_END T_NAME T_NL
	| T_FUN T_ASM T_NAME '[' Parameter_list ']'
		Assembler_blk       T_END T_NAME T_NL
	;

Decl_Type: T_TYPE T_NAME Decl_list
	;

Declaration
	: T_NL
	| Decl_Type
	| Decl_Sub
	| Decl_Fun
	| Decl_Operator
	| Decl_Symbol_list
	;

Decl_list
	: Declaration
	| Decl_list Declaration
	;


/******************************************************************************/
//                             PARSE STATEMENTS
/******************************************************************************/


Call_sub: %empty
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

For: %empty
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
	| Boolean  T_NL
	;


/******************************************************************************/
//                                     PARSE
/******************************************************************************/


Statement_list
	: Statement
	| Statement_list Statement
	;

unit
	: Decl_list T_EOF
	| Statement_list T_EOF
	| Decl_list Statement_list T_EOF
	;


