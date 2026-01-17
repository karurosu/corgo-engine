//
//  ecs/core/systems/debug_system.c
//  Simple system used for debugging purposes.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_DEBUG_SYSTEM, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)\
{
    CE_Debug("CE_CORE_DEBUG_SYSTEM running. Delta Time: %f\n", deltaTime);
    debugComponent->m_debugValue++;
}
CE_END_SYSTEM_IMPLEMENTATION
