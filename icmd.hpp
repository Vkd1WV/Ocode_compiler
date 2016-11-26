/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

//typedef union {
//	sym_pt s; ///< a variable
//	umax   l; ///< a literal
//} i_arg;

class icmd {
	private:
	name_dx label; ///< The label, if any, for this operation
	op_code op;    ///< The intermediate operator
	
	sym_pt result; ///< result of the operation
	sym_pt arg1;   ///< first argument
	sym_pt arg2;   ///< second argument
	
	name_dx jmp_targt; ///< target of a jump
	
	// Used by code generators
	bool result_live;
	bool arg1_live;
	bool arg2_live;
	
	public:
	icmd(
		name_dx      label,
		op_code      op,
		name_dx      target,
		const sym_pt out,
		const sym_pt left,
		const sym_pt right
	);
	void          Print(void) const;
	
	// getters
	const name_dx get_label(void) const;
	const op_code get_op   (void) const;
};


