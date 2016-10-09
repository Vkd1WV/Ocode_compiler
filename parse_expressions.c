/******************************************************************************/
// parse_expressions.c
// handles expression parsing and variable assignment of the omega language
//
// Ammon Dodson
// Spring 2016
/******************************************************************************/

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


// expressions in order of precedence
const sym_entry * Primary   (void);
const sym_entry * Unary     (void);
const sym_entry * Term      (void);
const sym_entry * Expression(void);
const sym_entry * Equation  (void);
const sym_entry * Boolean   (void);

void Assign(const sym_entry * in);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


/*	If we implicitly dereference symbols we will need reference and dereference operators. Primary() will not be able to directly write the the output file because we may have to reference the label. We will not be able to decide to call a function name until Term().
*/

/*	In Omnicode Variable names are label names using them directly returns the address where the data is stored. Assignments require that address to be able to change the data at that location. However, all other uses tend to need the data rather than its location and so should be dereferenced. The labels themselves obviously cannot be modifed.

@A + @B : C

*/

/*	Whatever the syntax, unary will need information about the type of symbol, 
	therefore Primary must make no changes to the registers and return only a 
	symbol entry.
*/

const sym_entry * Primary(void){
	static sym_entry symbol;
	const sym_entry * sym_pt;
	
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		sym_pt=Assignment_Statement();
		Match(T_CPAR);
		return sym_pt;
	case T_NUM:
		symbol.constant = true;
		symbol.value    = get_num();
		return &symbol;
	case T_NAME:
		if(!( sym_pt=iview(global_symbols,get_name()) ))
			error("Undeclared symbol");
		
		return sym_pt;
	default:
		printf(
			"ERROR: could not match token: '0x%02x' on line %d to any rule\n",
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
}

/*	Since multible unaries can be stacked each should know the type of the
	subordinate operation.
*/

const sym_entry * Unary(void){
	const sym_entry * arg;
	const sym_entry * result;
	
	switch (token){
	case T_MINUS:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		
		emit_triple("neg", result, arg);
		return result;
	
	case T_DREF:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		if (arg->constant) error("cannot dereference a constant");
		if (!arg->dref) error("cannot dereference a void pointer");
		
		emit_triple("@", result, arg);
		return result;
	
	case T_NOT:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		
		emit_triple("not", result, arg);
		return result;
	
	case T_INV:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		
		emit_triple("inv", result, arg);
		return result;
	
	default:
		arg = Primary();
		return arg;
	}
}

const sym_entry * Term(void){
	const sym_entry *arg1, *arg2;
	const sym_entry * result;
	
	arg1=Unary();
	//FIXME: what if arg1 is a constant?
	
	while(token>=T_TIMES && token<=T_RSHFT){
		switch(token){
		case T_TIMES:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("*", result, arg1, arg2);
		break;
		
		case T_DIV:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("/", result, arg1, arg2);
		break;
		
		case T_MOD:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("mod", result, arg1, arg2);
		break;
		
		case T_EXP:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("^", result, arg1, arg2);
		break;
		case T_LSHFT:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("lsh", result, arg1, arg2);
		break;
		case T_RSHFT:
			get_token();
			arg2=Unary();
			result = new_var();
			
			emit_quad("rsh", result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}

const sym_entry * Expression(void){
	const sym_entry *arg1, *arg2;
	const sym_entry * result;
	
	arg1=Term();
	
	while (token>=T_PLUS && token<=T_BXOR){
		switch (token){
		case T_PLUS:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("+", result, arg1, arg2);
		break;
		
		case T_MINUS:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("-", result, arg1, arg2);
		break;
		
		case T_BAND:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("and", result, arg1, arg2);
		break;
		
		case T_BOR:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("or", result, arg1, arg2);
		break;
		
		case T_BXOR:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("xor", result, arg1, arg2);
		break;
		}
		arg1 = result;
	}
	return arg1;
}

const sym_entry * Equation(void){
	const sym_entry *arg1, *arg2;
	const sym_entry * result;
	
	arg1 = Expression();
	
	while (token>=T_EQ && token<=T_GTE){
		switch (token){
		case T_EQ:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("=", result, arg1, arg2);
			break;
		case T_NEQ:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("neq", result, arg1, arg2);
			break;
		case T_LT:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("lt", result, arg1, arg2);
			break;
		case T_GT:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("gt", result, arg1, arg2);
			break;
		case T_LTE:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("lte", result, arg1, arg2);
			break;
		case T_GTE:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad("gte", result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}

const sym_entry * Boolean(void){
	const sym_entry *arg1, *arg2;
	const sym_entry * result;
	
	arg1 = Equation();
	
	while(token == T_AND || token == T_OR){
		switch(token){
		case T_AND:
			get_token();
			arg2=Equation();
			result=new_var();
			
			emit_quad("&", result, arg1, arg2);
			break;
		case T_OR:
			get_token();
			arg2=Equation();
			result=new_var();
			
			emit_quad("|", result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}

void Assign(const sym_entry * in){
	const sym_entry * out;
	
	get_token();
	out = Unary();
	emit_triple(":", out, in);
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


const sym_entry * Assignment_Statement(void){
	const sym_entry * result;
	
	//emit_cmnt("An assignment Statement");
	result = Boolean();
	while (token == T_ASS) Assign(result);
	return result;
}


