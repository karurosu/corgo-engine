//
//  engine/core/display.c
//  Functions to manipulate the display in a platform agnostic way.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "display.h"
#include "platform.h"

int CE_GetDisplayWidth()
{
    // Not supporting display scaling
    return 400;
}

int CE_GetDisplayHeight()
{
    // Not supporting display scaling
    return 240;
}