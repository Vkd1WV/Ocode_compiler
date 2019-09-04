# The Omega Code Compiler

This was one of my language design experiments.

##Usage

Usage: occ [OPTION]... FILE

A first cross compiler for O-code.

  -h, --help            display this help and exit
  -V, --version         output version information and exit
  -v                    be verbose. -vv produces tons of debugging output.
  -q                    be less verbose suppress all warnings. -qq silently fail on errors
  -d                    produce debug file.
  -D NAME               VALUE   Initialize an external constant. If no value is given it is assumed to be 1.
  -p                    produce portable executable
      --x86-long        produce assembler for x86 Long Mode
      --x86-protected   produce assembler for x86 Protected Mode
      --arm-v7
      --arm-v8


## Components

The software architecture looks roughly like this.

`oc file` -> scanner -> `token`

`token`   -> parser  -> `Program_data`

`Program_data` -> optomizer -> `Program_data`

`Program_data` -> pexe -> `pexe file`

`Program_data` -> arm  -> `arm.asm`

`Program_data` -> x86  -> `x86.asm`

I had intended to output a NASM compatible assembler file. As it stands it outputs a text representation of its intermediate data structures in a debugging file if asked for (-d).

I consider this project closed and unfinished.




