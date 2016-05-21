%token T_NL T_EOF

%token T_NAME T_NUM //primary
%token T_OPAR T_CPAR

%token T_INC T_DEC T_REF T_DREF T_NOT T_INV  // factor
%token T_TIMES T_MODUL T_DIV T_LSHFT T_RSHFT // term
%token T_PLUS T_MINUS T_BAND T_BOR T_BXOR    // expression
%token T_EQ T_NEQ T_LT T_GT T_LTE T_GTE      // equation
%token T_AND T_OR
%token T_ASS

%token T_LBL T_JMP T_IF T_ELSE T_WHILE

%%

primary
	: T_OPAR boolean T_CPAR
	| T_NUM
	| T_CH
	| T_NAME
	;

unary
	: T_INC   primary
	| T_DEC   primary
	| T_REF   primary
	| T_DREF  primary
	| T_NOT   primary
	| T_INV   primary
	| T_MINUS primary
	| primary
	;

term
	: factor
	| term T_TIMES factor
	| term T_MODUL factor
	| term T_DIV   factor
	| term T_LSHFT factor
	| term T_RSHFT factor
	;

expression
	: term
	| expression T_PLUS  term
	| expression T_MINUS term
	| expression T_BAND  term
	| expression T_BOR   term
	| expression T_BXOR  term
	;

equation
	: expression
	| equation T_EQ  expression
	| equation T_NEQ expression
	| equation T_LT  expression
	| equation T_GT  expression
	| equation T_LTE expression
	| equation T_GTE expression
	;

boolean
	: equation
	| boolean T_AND equation
	| boolean T_OR  equation
	;

assignment
	: boolean
	| boolean assignment_list
	;

assignment_list
	: assignment_op assignment
	| assignment_list assignment_op assignment
	;

assignment_op
	: T_ASS
	;


if	: T_IF assignment statement T_NL
	| T_IF assignment statement T_NL T_ELSE statement
	| 
	| T_IF assignment T_NL block
	| T_IF assignment T_NL block T_ELSE block
	;

declaration
	: data_type

statement  //each must terminate with NL
	: if
	| T_LBL T_NAME T_NL
	| T_JMP T_NAME T_NL
	| T_WHILE assignment
	| declaration
	| assignment T_NL
	;

statement_list
	: statement
	| statement_list statement
	;

block: T_NL statement_list;

unit: statement_list T_EOF;



