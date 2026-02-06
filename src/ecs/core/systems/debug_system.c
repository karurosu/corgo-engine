//
//  ecs/core/systems/debug_system.c
//  Simple system used for debugging purposes.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_ENTITY_DEBUG_SYSTEM, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)
{

}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_CORE_GLOBAL_DEBUG_SYSTEM)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_CORE_GLOBAL_DEBUG_COMPONENT, globalDebugComp);
    if (!globalDebugComp)
    {
        CE_Error("Failed to access global debug component");
        return CE_ERROR;
    }

    if (globalDebugComp->m_enabled)
    {
#ifdef CE_CORE_TEST_MODE
        globalDebugComp->m_testValue++;
#endif
    }
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION


