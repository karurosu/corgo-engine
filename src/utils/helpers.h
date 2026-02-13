//
//  utils/helpers.h
//  Basic C helpers used by the entire project.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_UTILS_HELPERS_H
#define CORGO_UTILS_HELPERS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

//// Helper macros to support static analysis annotations in the ARM compiler
#ifdef _In_
#define IN _In_
#define OUT _Out_
#define INOUT _Inout_
#define IN_OPT _In_opt_
#define OUT_OPT _Out_opt_
#define INOUT_OPT _Inout_opt_
#else
#define IN
#define OUT
#define INOUT
#define IN_OPT
#define OUT_OPT
#define INOUT_OPT
#endif

// Pasting helper
#define CE_PASTE_HELPER(x, y) x##y
#define CE_PASTE(x, y) CE_PASTE_HELPER(x, y)

// popcnt
#if(_WINDLL)
	#define CE_popcnt __popcnt
#else
	#define CE_popcnt __builtin_popcount
#endif

#endif // CORGO_UTILS_HELPERS_H
