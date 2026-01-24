//
//  engine/core/display.h
//  Functions to manipulate the display in a platform agnostic way.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_DISPLAY_H
#define CORGO_ENGINE_CORE_DISPLAY_H

#include <stdint.h>

// Get display size used for rendering.
// Returns an int to simplify PD API integration.
int CE_GetDisplayWidth();
int CE_GetDisplayHeight();

#endif // CORGO_ENGINE_CORE_DISPLAY_H