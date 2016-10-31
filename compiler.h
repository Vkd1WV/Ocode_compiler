#ifndef _COMPILER_H
#define _COMPILER_H


/******************************************************************************/
//                            Arbitrary Limits
/******************************************************************************/


#define UNQ_LABEL_SZ 18 // string length limit for compiler generated labels
#define NAME_MAX     64 // symbol name length limit.
#define ERR_ARR_SZ  100 // temp arrays used in error reporting


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
//#include "byte_codes.h"
#include "globals.h"
#include "functions.h"


#endif // _COMPILER_H
