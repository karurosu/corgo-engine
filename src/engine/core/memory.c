//
//  engine/core/memory.c
//  Memory management tools.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdlib.h>

#include "memory.h"

void *CE_realloc(void *ptr, size_t size)
{
#ifdef CE_BACKEND_PLAYDATE
    return CE_GetPlaydateAPI()->system->realloc(ptr, size);
#else
    return realloc(ptr, size);
#endif
}

void CE_free(void *ptr)
{
#ifdef CE_BACKEND_PLAYDATE
    CE_GetPlaydateAPI()->system->realloc(ptr, 0);
#else
    free(ptr);
#endif
}