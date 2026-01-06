//
//  engine/core/memory.h
//  Memory management tools.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_MEMORY_H
#define CORGO_ENGINE_CORE_MEMORY_H

#include <stddef.h>
#include <stdint.h>

#include "platform.h"

void *CE_realloc(void *ptr, size_t size);
void CE_free(void *ptr);

// CC memory management macros
#define CC_REALLOC CE_realloc
#define CC_FREE CE_free

#endif // CORGO_ENGINE_CORE_MEMORY_H