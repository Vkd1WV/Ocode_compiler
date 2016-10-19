#ifndef _TYPES_H
#define _TYPES_H

/******************************************************************************/
//                            Type Definitions
/******************************************************************************/


typedef unsigned long long umax;
typedef unsigned int uint;

typedef uint16_t token_t;

typedef enum {Real, Protected, Virtual, SMM, Compatibility, Long} mode_t;
typedef enum {x86, arm} arch_t;

//typedef enum{
//	IMM,
//	R0, R1, R2 , R3 , R4 , R5 , R6 , R7 ,
//	//R8, R9, R10, R11, R12, R13, R14,
//	R15,
//	// x86
//	A , B , C  , D  , SI , DI , BP ,
//	// arm
//	//A1, A2, A3 , A4 , V1 , V2 , V3 , V4 ,
//	//V5, V6, V7 , V8 , WR , SB , SL , FP ,
//	//IP, LR, PC,
//	SP // stack pointer x86=R7, arm=R13
//} reg_t;

/********************************* SYMBOLS ************************************/
// Each symbol table entry contains all data related to named symbols in the 
// program. Some symbols reprsent an address location. The symbol table entry
// indicates what is at that location. 
// Other symbols like typedefs or constants only exist at compile time.


/*	Variable
		assume zero initialization
		insert symbol when used
	Initialized variable
		insert symbol when used
	declared const
		insert value when used
	immediate constant
		insert value when used
*/

/*	SYMBOLS
RUN-TIME
	Variable
		Datatype
			size of the memory location
		Initial value
		static or dynamic memory
		whether assignments are allowed in the current context
	Subroutine
		parameter specification
		local symbols
	Function
		parameter specification
		local symbols
		return datatype
	pointer
		how many derefs
		datatype pointed to
		static or dynamic memory
		whether assignments are allowed and at which level of dereference
COMPILE-TIME
	Literal values and declared constants
		Datatype
		value
		Just insert the value when used
	Type Definition
		??
*/

typedef enum {
	none,
	temp,
	data,
	pointer,
	function,
	subroutine,
	literal,
	type_def
	//defined type
} symbol_t;

//switch (sym_pt->type){
//case none:
//case psudo_reg:
//case data:
//case pointer:
//case function:
//case subroutine:
//case literal:
//case type_def:
//default:
//}

typedef struct sym {
	char     name[NAME_MAX];
	symbol_t type;     // Symbol type
	
	// Data, Pointer
	bool stat;     // Is it a static data location
	bool constant; // should this data ever be changed again
	bool init;     // Is the data location initialized
	
	// Data and temp
	umax size; // size in bytes
	
	// Data, pointer, or literal
	umax value; // Initialized value
	
	// function and subroutine
	bool assembler; // Is this an assembler routine fun or sub
	DS   local;     // Local scope for functions and structures
	// Parameter specification
//	sym_entry * A;
//	sym_entry * B;
//	sym_entry * C;
//	sym_entry * D;
//	sym_entry * E;
//	sym_entry * F;
	// return value
	
	// pointer
	struct sym* dref;
}sym_entry;


/*******************************  ************************************/

typedef struct icode {
	uint16_t cmd;
	sym_entry* result;
	sym_entry* arg1;
	sym_entry* arg2;
} icmd;

#endif // _TYPES_H

