#ifndef _COMPILER_H
#define _COMPILER_H


/******************************************************************************/
//                            Arbitrary Limits
/******************************************************************************/


#define UNQ_LABEL_SZ 12 // string lngth limit for compiler generated labels
#define NAME_MAX     64 // symbol name length limit. not enforced
#define PT_SIZE      20 // arrays used in emmiters


/******************************************************************************/
//                            Collected Headers
/******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <data.h>

#include "types.h"
#include "tokens.h"
#include "globals.h"
#include "functions.h"


#endif // _COMPILER_H
