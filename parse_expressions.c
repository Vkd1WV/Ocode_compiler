/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


// expressions in order of precedence
const sym_entry * Primary   (void);
const sym_entry * Unary     (void);
const sym_entry * Postfix   (void);
const sym_entry * Term      (void);
const sym_entry * Expression(void);
const sym_entry * Equation  (void);


sym_entry * Assign(sym_entry * target);


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


/*	Variables must be implicitly dereferenced.
	Constants must have their value inserted.
	Functions must be implicitly called and their return value substituted in.
*/

const sym_entry * Primary(void){
	sym_entry * sym;
	char msg_arr[ERR_ARR_SZ];
	
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		sym=Boolean();
		Match(T_CPAR);
		break;
		
	case T_NUM:
		sym = new_var();
		sym->type  = literal;
		sym->init  = true;
		sym->value = get_num();
		break;
		
	case T_NAME:
		if(!( sym = DS_find(symbols, get_name()) ))
			parse_error("Undeclared symbol");
		
		if(sym->type == function  ){} // get the function's return value
		else if(sym->type == subroutine){} // call the subroutine
		else if(sym->type == type_def  ){} // this is actually a declaration
		break;
		
	default:
		sprintf(msg_arr, "Could not match token: '%s' to any rule", yytext);
		parse_error(msg_arr);
		__builtin_unreachable ();
	}
	
	return sym;
}


const sym_entry * Unary(void){
	const sym_entry * arg;
	sym_entry * result;
	
	switch (token){
	case T_MINUS:
		get_token();
		arg = Unary();
		
		// Symantic Checks
		switch (arg->type){
		case literal:
		case data: break;
		case subroutine: parse_error("Subroutine used in an expression");
		case none:       parse_error("Trying to negate a void data type");
		case type_def:   parse_error("Data type used in an expression");
		default:     crit_error("Internal Compiler Error at Unary(), T_MINUS");
		}
		
		result = new_var();
		
		emit_iop(I_NEG, 0, result, arg, NULL);
		return result;
	
	case T_REF:
		get_token();
		arg = Primary();
		result = new_var();
		
		// Symantic Checks
		switch (arg->type){
		case none: crit_error("Symbol has no type");
		case temp: parse_error("Can only create a reference to a memory location");
		case data:
		case pointer:
		case function:
		case subroutine: break;
		case literal: parse_error("Can only create a reference to a memory location");
		case type_def: parse_error("Data type used in an expression");
		default: crit_error("Internal Compiler Error at Unary(), T_REF");
		}
		
		result->type = pointer;
		result->dref = arg;
		
		emit_iop(I_REF, 0, result, arg, NULL);
		return result;
	
	case T_DREF:
		get_token();
		arg = Unary();
		
		// Symantic Checks
		if(!arg->dref) parse_error("Trying to dereference a void data type");
		
		result = arg->dref;
		
		emit_iop(I_DREF, 0, result, arg, NULL);
		return result;
	
	case T_NOT:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		
		emit_iop(I_NOT, 0, result, arg, NULL);
		return result;
	
	case T_INV:
		get_token();
		arg = Unary();
		result = new_var();
		
		// Symantic Checks
		
		emit_iop(I_INV, 0, result, arg, NULL);
		return result;
	
	default:
		arg = Primary();
		return arg;
	}
}


const sym_entry * Postfix(void){
	const sym_entry * arg;
	//sym_entry * result;
	
	arg=Unary();
	
	// Make navigation operators '.' '[]' implicitly safe if the target is null
	
	while (false){
		switch (token){
		
		}
	}
	
	return arg;
}


const sym_entry * Term(void){
	const sym_entry *arg2;
	      sym_entry *arg1, *result;
	
	arg1=Postfix();
	
	// Handle assignment statements
	if (token >= T_ASS && token <= T_XOR_A)
		arg1 = Assign(arg1);
	
	while(token>=T_MUL && token<=T_RSHFT){
		switch(token){
		case T_MUL:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_MUL, 0, result, arg1, arg2);
		break;
		
		case T_DIV:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_DIV, 0, result, arg1, arg2);
		break;
		
		case T_MOD:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_MOD, 0, result, arg1, arg2);
		break;
		
		case T_EXP:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_EXP, 0, result, arg1, arg2);
		break;
		case T_LSHFT:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_LSH, 0, result, arg1, arg2);
		break;
		case T_RSHFT:
			get_token();
			arg2=Postfix();
			result = new_var();
			
			emit_iop(I_RSH, 0, result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}

const sym_entry * Expression(void){
	const sym_entry *arg1, *arg2;
	sym_entry * result;
	
	arg1=Term();
	
	while (token>=T_PLUS && token<=T_BXOR){
		switch (token){
		case T_PLUS:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_iop(I_ADD, 0, result, arg1, arg2);
		break;
		
		case T_MINUS:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_iop(I_SUB, 0, result, arg1, arg2);
		break;
		
		case T_BAND:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_iop(I_BAND, 0, result, arg1, arg2);
		break;
		
		case T_BOR:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_iop(I_BOR, 0, result, arg1, arg2);
		break;
		
		case T_BXOR:
			get_token();
			arg2=Term();
			result = new_var();
			
			emit_iop(I_XOR, 0, result, arg1, arg2);
		break;
		}
		arg1 = result;
	}
	return arg1;
}

const sym_entry * Equation(void){
	const sym_entry *arg1, *arg2;
	sym_entry * result;
	
	arg1 = Expression();
	
	while (token>=T_EQ && token<=T_GTE){
		switch (token){
		case T_EQ:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_EQ, 0, result, arg1, arg2);
			break;
		case T_NEQ:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_NEQ, 0, result, arg1, arg2);
			break;
		case T_LT:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_LT, 0, result, arg1, arg2);
			break;
		case T_GT:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_GT, 0, result, arg1, arg2);
			break;
		case T_LTE:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_LTE, 0, result, arg1, arg2);
			break;
		case T_GTE:
			get_token();
			arg2=Expression();
			result=new_var();
			
			emit_iop(I_GTE, 0, result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}


sym_entry * Assign(sym_entry * target){
	const sym_entry * result;
	token_t op;
	
	if(target->type != data && target->type != pointer)
		parse_error("Invalid target of an assignment");
	if(target->constant) parse_error("cannot make an assignment to a constant");
	
	target->init = true;
	
	op=token;
	get_token();
	result=Boolean();
	
	if (!result->init) parse_error("Using an uninitialized value");
	
	switch (op){
	case T_ASS: emit_iop(I_ASS, 0, target, result, NULL); break;
	case T_LSH_A:
	case T_RSH_A:
	case T_ADD_A:
	case T_SUB_A:
	case T_MUL_A:
	case T_DIV_A:
	case T_MOD_A:
	case T_AND_A:
	case T_OR_A:
	case T_XOR_A: parse_error("Feature not implemented");
	default: crit_error("Internal Compiler Error at Assign()");
	}
	
	return result;
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


const sym_entry * Boolean(void){
	const sym_entry *arg1, *arg2;
	sym_entry * result;
	
	arg1 = Equation();
	
	while(token == T_AND || token == T_OR){
		switch(token){
		case T_AND:
			get_token();
			arg2=Equation();
			result=new_var();
			
			emit_iop(I_AND, 0, result, arg1, arg2);
			break;
		case T_OR:
			get_token();
			arg2=Equation();
			result=new_var();
			
			emit_iop(I_OR, 0, result, arg1, arg2);
		}
		arg1 = result;
	}
	return arg1;
}


