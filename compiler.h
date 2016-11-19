/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _COMPILER_H
#define _COMPILER_H


/******************************************************************************/
//                            Arbitrary Limits
/******************************************************************************/


/// string length limit for unique compiler generated labels
#define UNQ_NAME_SZ 16
#define ERR_ARR_SZ  100  ///< temp arrays used in error reporting
#define NAME_ARR_SZ 1024 ///< Starting size for the dynamic name array

const char default_outfile[8] = "out.asm";


/******************************************************************************/
//                            Collected Headers
/******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <data.h>

#include "types.h"
#include "tokens.h"
#include "globals.h"
#include "functions.h"


#endif // _COMPILER_H
