/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _MY_TYPES_H
#define _MY_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

// Signed
typedef long long int imax;

#define IMAX_MAX LLONG_MAX
#define IMAX_MIN LLONG_MIN

// Unsigned
typedef unsigned long long int umax;
typedef unsigned int uint;

#define UMAX_MAX ULLONG_MAX
// INT_MAX is defined in limits.h

#endif // _MY_TYPES_H


