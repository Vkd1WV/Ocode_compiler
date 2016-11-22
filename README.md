# The Omega Code Compiler

I'm proving I can build my own programming language. I am not currently licencing this software in any way.

##Usage: occ [OPTIONS]... FILE...

###Debugging
  -v   be verbose

###External Constant Definitions
  -D NAME=VALUE   Initialize an external constant. If no value is given it is assumed to be 1.

###Output Options
  --outfile=FILE Redirect the output to a different file.
  -d             produce debug file.
  -p             produce portable executable
  -a             produce assembler

###Target Architecture Options
  --x86-long      Build for x86 Long Mode
  --x86-protected Build for x86 Protected Mode
  --arm-v7
  --arm-v8

## Architecture

### Components

`oc file` -> scanner -> `token`
`token`   -> parser  -> `name_array, symbols, inst_q, & dbg file`

Optionally an optomizer could be here

`name_array, symbols, inst_q` -> pexe -> `pexe file`
`name_array, symbols, inst_q` -> arm  -> `arm.asm`
`name_array, symbols, inst_q` -> x86  -> `x86.asm`

For simplicity assembly and linking are not built-in.

## OCode

I want a type checking mode between one type, and void (any type), such that I can list the allowable types. particularly for function calls, so one parameter can be used to interpret what the others should be. Like a union without having to wrap each parameter in a union before passing it.

Enumerated types should each be in their own namespace

## Intermediate Code
The intermetiate represetation consists of three parts:
*	The name array
*	The symbol table
*	The instruction queue

The name array is a dynamically sized array containing all the user and compiler defined names. I can also include all the program's string literals.

The symbol table contains each variable's declaration information including types and sizes and an index into the name array.

The op-code queue contains a series of three-address op-codes:

Binary Operators

OP	RSLT	ARG1	ARG2

Unary Operators

OP	RSLT	ARG

Flow Control

LBL
JMP
CJMP	CND	LBL

parameter pass
call function
return function

 # literal numbers
 % temp variables / register place holders.

## Other junk

What if all declared types were classes
	could there be implicit member selection if there is only one member?

the only fundamental operations are
	declarations
		constants
		variables
		classes
		functions
	assignments
	reference / dereference
	function calls
	label / jump
	if / else

for simplicity we need to assume only one data type: unsigned qword

First:
	variable declarations (one type)
	assignments
	expressions

Second:
	function calls

fourth:
	ref / deref (arithmetic depends on typesizes)?

REFERENCES
*	With Ref operator
	*	variables must be implicitly dereferenced
	*	reference passing must be done explicitly
*	Without ref operator
	*	all variables need to be dereferenced to get their value
	*	constants do not need to be dereferenced, but may for consistancy
	*	passing anything to a function always passes the reference allows us to make explicit whether they are in or out.


