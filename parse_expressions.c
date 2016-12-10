/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


/******************************************************************************/
//                           PRIVATE FUNCTIONS
/******************************************************************************/


/*	Variables must be implicitly dereferenced.
	Constants must have their value inserted.
	Functions must be implicitly called and their return value substituted in.
*/

static inline bool is_init(sym_pt sym){
	if(!sym) puts("ERROR: is_init() received a NULL");
	
	if(!sym->init){
		parse_warn("Using uninitialized symbol");
		Print_sym(stderr, sym);
		return false;
	}
	return true;
}

static inline void set_init_size(sym_pt result, sym_pt arg1, sym_pt arg2){
	char message[ERR_ARR_SZ];
	
	// check initialization
	if(!arg1->init){
		parse_warn("Using uninitialized symbol");
		Print_sym(stderr, arg1);
	}
	else if(arg2 && !arg2->init){ // arg2 may be NULL
		parse_warn("Using uninitialized symbol");
		Print_sym(stderr, arg2);
	}
	else result->init = true;
	
	if(arg1->type == st_ref || (arg2 && arg2->type == st_ref))
		if(result->type != st_ref) {
			sprintf(
				message,
				"Assigning reference to non-reference %s",
				dx_to_name(result->name)
			);
			parse_warn(message);
		}
	
	// check size
	if(result->type == st_int){
		
	}
	
	// handle if one arg is literal
	// can't really make direct comparison
	
}


static sym_pt Primary(void){
	sym_pt sym;
	char msg_arr[ERR_ARR_SZ];
	
	switch (token){
	case T_OPAR:
		Match(T_OPAR);
		sym=Boolean();
		Match(T_CPAR);
		break;
		
	case T_CHAR:
	case T_NUM:
		sym = new_var(st_lit_int);
		sym->init     = true;
		sym->constant = true;
		sym->value    = get_num();
		break;
		
	case T_NAME:
		if(!( sym = (sym_pt) DS_find(symbols, get_name()) ))
			parse_error("Undeclared symbol");
		
		if(sym->type == st_fun){
			// Function call
			// sym = return value
		}
		break;
		
	case T_STR:
	default:
		sprintf(msg_arr, "Could not match token: '%s' to any rule", yytext);
		parse_error(msg_arr);
		__builtin_unreachable ();
	}
	
	return sym;
}


static sym_pt Unary(void){
	sym_pt arg, result;
	const char * ref_inval = "Invalid Target of Reference";
	
	switch (token){
/*	case T_MINUS:*/
/*		get_token();*/
/*		arg = Unary();*/
/*		*/
/*		*/
/*		*/
/*		// Symantic Checks*/
/*		switch (arg->type){*/
/*		case literal:*/
/*			*/
/*			*/
/*			break;*/
/*		*/
/*		case data:*/
/*			result = new_var();*/
/*			emit_iop(NO_NAME, I_NEG, NO_NAME, result, arg, NULL);*/
/*			return result;*/
/*		*/
/*		case subroutine: parse_error("Subroutine used in an expression");*/
/*		case none:       parse_error("Trying to negate a void data type");*/
/*		case type_def:   parse_error("Data type used in an expression");*/
/*		default:         parse_crit(arg1, arg2, "Internal: at Unary(), T_MINUS");*/
/*		}*/
	
	case T_REF:
		get_token();
		arg = Primary();
		
		
		// Symantic Checks
		if(arg->temp) parse_error(ref_inval);
		switch (arg->type){
		case st_int:
		case st_ref:
		case st_fun:
		case st_sub: break;
		case st_lit_int:
		case st_lit_str:
		case st_type_def: parse_error(ref_inval);
		case st_undef:
		case st_NUM:
		default: parse_crit(arg, NULL, "Internal: at Unary(), T_REF");
		}
		
		result = new_var(st_ref);
		result->init = true;
		result->dref = arg;
		
		emit_iop(NO_NAME, I_REF, NO_NAME, result, arg, NULL);
		return result;
	
	case T_DREF:
		get_token();
		arg = Unary();
		
		
		// Symantic Checks
		if(arg->type != st_ref) parse_error("Invalid Target of Dereference");
		if(!arg->init) parse_error("Dereferencing uninitialized pointer");
		if(!arg->dref) parse_error("Trying to dereference a void pointer");
		
		result = arg->dref;
		
		emit_iop(NO_NAME, I_DREF, NO_NAME, result, arg, NULL);
		return result;
	
	case T_NOT:
		get_token();
		arg = Unary();
		
		
		// Symantic Checks
		switch (arg->type){
		case st_int:
			result = new_var(st_int);
			set_init_size(result, arg, NULL);
			emit_iop(NO_NAME, I_NOT, NO_NAME, result, arg, NULL);
			break;
		
		case st_lit_int:
			arg->value = !arg->value;
			result = arg;
			break;
		
		case st_ref:
		case st_fun:
		case st_sub:
		case st_lit_str:
		case st_type_def: parse_error("Invalid target of NOT operation");
		case st_undef:
		case st_NUM:
		default: parse_crit(arg, NULL, "Internal: at Unary(), T_NOT");
		}
		
		return result;
	
	case T_INV:
		get_token();
		arg = Unary();
		
		
		// Symantic Checks
		switch (arg->type){
		case st_int:
			result = new_var(st_int);
			set_init_size(result, arg, NULL);
			emit_iop(NO_NAME, I_INV, NO_NAME, result, arg, NULL);
			break;
		
		case st_lit_int:
			arg->value = ~arg->value;
			result = arg;
			break;
		
		case st_ref:
		case st_fun:
		case st_sub:
		case st_lit_str:
		case st_type_def: parse_error("Invalid target of NOT operation");
		case st_undef:
		case st_NUM:
		default: parse_crit(arg, NULL, "Internal: at Unary(), T_NOT");
		}
		
		return result;
	
	default:
		arg = Primary();
		return arg;
	}
}


static sym_pt Postfix(void){
	sym_pt arg;
	//sym_pt result;
	
	arg=Unary();
	
	// Make navigation operators '.' '[]' implicitly safe if the target is null
	
	while (false){
		switch (token){
		default: break;
		}
	}
	
	return arg;
}


static sym_pt Term(void){
	sym_pt arg2, arg1, result;
	
	arg1=Postfix();
	
	// Handle assignment statements
	if (token >= T_ASS && token <= T_XOR_A) arg1 = Assign(arg1);
	// since assignment called Boolean which already returned we know we are at
	// the end of this expression.
	
	while(token>=T_MUL && token<=T_RSHFT){
		switch(token){
		case T_MUL:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to multiplication");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to multiplication");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value * arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_MUL, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		case T_DIV:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to division");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to division");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value / arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_DIV, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		case T_MOD:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to modulus");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to modulus");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value % arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_MOD, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		case T_EXP:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to exponentiation");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to exponentiation");
			
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = new_var(st_lit_int);
				result->init = true;
				result->constant = true;
				
				if(!arg2->value) result->value = 0;
				else{
					result->value = arg1->value;
					for(umax i = arg2->value; i > 1; i--)
						result->value *= arg1->value;
				}
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_EXP, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		case T_LSHFT:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to left shift");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to left shift");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value << arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_LSH, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		case T_RSHFT:
			get_token();
			arg2=Postfix();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to right shift");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to right shift");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value >> arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_RSH, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		default:
			sprintf(err_array, "Internal: at Term() with token %d", token);
			parse_crit(arg1, arg2, err_array);
		}
		arg1 = result;
	}
	return arg1;
}

static sym_pt Expression(void){
	sym_pt arg1, arg2, result;
	
	arg1=Term();
	
	while (token>=T_PLUS && token<=T_BXOR){
		switch (token){
		case T_PLUS:
			get_token();
			arg2=Term();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to addition");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Expression(), T_PLUS");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to addition");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Expression(), T_PLUS");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value + arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				if (arg2->type == st_ref || arg1->type == st_ref)
					result = new_var(st_ref);
				else result = new_var(st_int);
				
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_ADD, NO_NAME, result, arg1, arg2);
			}
		break;
		
		
		case T_MINUS:
			get_token();
			arg2=Term();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to subtraction");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Expression(), T_MINUS");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to subtraction");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Expression(), T_MINUS");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value - arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				if (arg2->type == st_ref || arg1->type == st_ref)
					result = new_var(st_ref);
				else result = new_var(st_int);
				
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_SUB, NO_NAME, result, arg1, arg2);
			}
		break;
		
		case T_BAND:
			get_token();
			arg2=Term();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to binary and");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to binary and");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value & arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_BAND, NO_NAME, result, arg1, arg2);
			}
		break;
		
		case T_BOR:
			get_token();
			arg2=Term();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to binary or");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to binary or");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value | arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_BOR, NO_NAME, result, arg1, arg2);
			}
			break;
		
		case T_BXOR:
			get_token();
			arg2=Term();
			
			// Symantic Checks
			if(arg1->type != st_int && arg1->type != st_lit_int)
				parse_error("Invalid first argument to exclusive binary or");
			if(arg2->type != st_int && arg2->type != st_lit_int)
				parse_error("Invalid second argument to exclusive binary or");
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = arg1->value ^ arg2->value;
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				set_init_size(result, arg1, arg2);
				emit_iop(NO_NAME, I_XOR, NO_NAME, result, arg1, arg2);
			}
			break;
		
		default: parse_crit(arg1, arg2, "Internal: at Expression()");
		}
		
		arg1 = result;
	}
	return arg1;
}

static sym_pt Equation(void){
	sym_pt arg1, arg2, result;
	
	arg1 = Expression();
	
	while (token>=T_EQ && token<=T_GTE){
		switch (token){
		case T_EQ:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to equality");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_EQ");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to equality");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_EQ");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value == arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_EQ, NO_NAME, result, arg1, arg2);
			}
			break;
			
			
		case T_NEQ:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to not equal");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_NEQ");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to not equal");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_NEQ");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value != arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_NEQ, NO_NAME, result, arg1, arg2);
			}
			break;
			
			
		case T_LT:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to less than");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_LT");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to less than");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_LT");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value < arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_LT, NO_NAME, result, arg1, arg2);
			}
			break;
			
			
		case T_GT:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to greater than operator");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_GT");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to greater than operator");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_GT");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value > arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_GT, NO_NAME, result, arg1, arg2);
			}
			break;
			
			
		case T_LTE:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to lte");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_LTE");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to lte");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_LTE");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value <= arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_LTE, NO_NAME, result, arg1, arg2);
			}
			break;
			
			
		case T_GTE:
			get_token();
			arg2=Expression();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to gte");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_GTE");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to gte");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Equation(), T_GTE");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value >= arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(is_init(arg1) && is_init(arg2)) result->init = true;
				emit_iop(NO_NAME, I_GTE, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		default: parse_crit(arg1, arg2, "Internal: at Equation()");
		}
		
		Warn_comparison(arg1, arg2);
		arg1 = result;
	}
	return arg1;
}


static sym_pt Assign(sym_pt target){
	sym_pt result;
	token_t op;
	
	if(target->type != st_int && target->type != st_ref)
		parse_error("Invalid target of an assignment");
	if(target->constant) parse_error("cannot make an assignment to a constant");
	
	op=token;
	get_token();
	result=Boolean();
	
	// FIXME: this check is bad
	if (result->size > target->size)
		parse_warn("Assignment target is of a smaller type than the expression");
	if (!is_init(result)) parse_error("Assignment from an uninitialized value");
	
	set_init_size(target, result, NULL);
	
	switch (op){
	case T_ASS:
		emit_iop(NO_NAME, I_ASS, NO_NAME, target, result, NULL  );
		break;
	
	case T_LSH_A:
		emit_iop(NO_NAME, I_LSH, NO_NAME, target, target, result);
		break;
	
	case T_RSH_A:
		emit_iop(NO_NAME, I_RSH, NO_NAME, target, target, result);
		break;
	
	case T_ADD_A:
		emit_iop(NO_NAME, I_ADD, NO_NAME, target, target, result);
		break;
	
	case T_SUB_A:
		emit_iop(NO_NAME, I_SUB, NO_NAME, target, target, result);
		break;
	
	case T_MUL_A:
		emit_iop(NO_NAME, I_MUL, NO_NAME, target, target, result);
		break;
	
	case T_DIV_A:
		emit_iop(NO_NAME, I_DIV, NO_NAME, target, target, result);
		break;
	
	case T_MOD_A:
		emit_iop(NO_NAME, I_MOD, NO_NAME, target, target, result);
		break;
	
	case T_AND_A:
		emit_iop(NO_NAME, I_AND, NO_NAME, target, target, result);
		break;
	
	case T_OR_A :
		emit_iop(NO_NAME, I_OR , NO_NAME, target, target, result);
		break;
	
	case T_XOR_A:
		emit_iop(NO_NAME, I_XOR, NO_NAME, target, target, result);
		break;
	
	default: parse_crit(target, result, "Internal: at Assign()");
	}
	
	target->init = true;
	
	return target;
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


sym_pt Boolean(void){
	sym_pt arg1, arg2, result;
	
	arg1 = Equation();
	
	while(token == T_AND || token == T_OR){
		switch(token){
		case T_AND:
			get_token();
			arg2=Equation();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to logical and");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Boolean(), T_AND");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to logical and");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Boolean(), T_AND");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value && arg2->value);
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				result->size = w_byte;
				if(arg1->init && arg2->init) result->init = true;
				else parse_warn("Using an uninitialized value");
				emit_iop(NO_NAME, I_AND, NO_NAME, result, arg1, arg2);
			};
			break;
		
		
		case T_OR:
			get_token();
			arg2=Equation();
			
			// Symantic Checks
			switch (arg1->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid first argument to logical and");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Boolean(), T_AND");
			}
			
			switch (arg2->type){
			case st_int:
			case st_ref:
			case st_lit_int: break;
			case st_fun:
			case st_sub:
			case st_lit_str:
			case st_type_def:
				parse_error("Invalid second argument to logical and");
			case st_undef:
			case st_NUM:
			default: parse_crit(arg1, arg2, "Internal: at Boolean(), T_AND");
			}
			
			if(arg2->type == st_lit_int && arg1->type == st_lit_int){
				// fold the literals
				result = arg1;
				result->value = (arg1->value || arg2->value);
				
				if(DS_find(symbols, dx_to_name(arg2->name)))
					DS_remove(symbols);
			}
			else{
				result = new_var(st_int);
				if(arg1->init && arg2->init) result->init = true;
				else parse_warn("Using an uninitialized value");
				result->size = w_byte;
				emit_iop(NO_NAME, I_OR, NO_NAME, result, arg1, arg2);
			}
			break;
		
		
		default: parse_crit(arg1, arg2, "Internal: at Boolean()");
		}
		
		arg1 = result;
	}
	return arg1;
}


