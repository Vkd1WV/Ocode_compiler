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


/*	Variables must be implicitly dereferenced.
	Constants must have their value inserted.
	Functions must be implicitly called and their return value substituted in.
*/

const sym_entry * Primary(void){
	const sym_entry * in;
	sym_entry * out;
	
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		in=Assignment_Statement();
		Match(T_CPAR);
		return in;
	case T_NUM:
		out = new_var();
		out->type  = literal;
		out->value = get_num();
		return out;
	case T_NAME:
		/* Symbols cannot be implicitly derefernced here because then the reference would not be avilible for assignments
		*/
		if(!( in=iview(global_symbols,get_name()) ))
			error("Undeclared symbol");
		return in;
	default:
		printf(
			"ERROR: could not match token: '0x%02x' on line %d to any rule\n",
			token,
			yylineno
		);
		exit(EXIT_FAILURE);
	}
}


const sym_entry * Unary(void){
	const sym_entry * arg;
	const sym_entry * result;
	
	switch (token){
	case T_MINUS:
		get_token();
		arg = Unary();
		
		// Symantic Checks
		switch (arg->type){
		case literal:
		case variable: break;
		case subroutine: error("Subroutine used in an expression");
		case none:       error("Trying to negate a void data type");
		case type_def:   error("Data type used in an expression");
		default:         error("Internal Compiler Error at Unary(), T_MINUS");
		}
		
		result = new_var();
		
		emit_triple("neg", result, arg);
		return result;
	
	case T_REF:
		get_token();
		arg = Primary();
		//FIXME: Do something
		return arg;
	
	case T_DREF:
		get_token();
		arg = Unary();
		
		// Symantic Checks
		if(!arg->dref) error("Trying to dereference a void data type");
		
		result = arg->dref;
		
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
			
			emit_quad("&", result, arg1, arg2);
		break;
		
		case T_BOR:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_quad("|", result, arg1, arg2);
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
			
			emit_quad("<", result, arg1, arg2);
			break;
		case T_GT:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_quad(">", result, arg1, arg2);
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
			
			emit_quad("and", result, arg1, arg2);
			break;
		case T_OR:
			get_token();
			arg2=Equation();
			result=new_var();
			
			emit_quad("or", result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}

void Assign(const sym_entry * in){
	const sym_entry * out;
	
	get_token();
	out = Unary();
	
	// symantic checks
	if(out->constant) error("Cannot make an assignment to a constant");
	
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


