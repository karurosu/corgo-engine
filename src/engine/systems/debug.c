//
//  ecs/core/systems/debug_system.c
//  Debug systems.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifdef CE_DEBUG_BUILD

#include "engine/corgo.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_ENGINE_ENTITY_DEBUG_SYSTEM, CE_ENGINE_DEBUG_SYSTEM_DEPENDENCIES)
{
    // TODO
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_GLOBAL_DEBUG_SYSTEM)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_CORE_GLOBAL_DEBUG_COMPONENT, globalDebugComponent);
    if (globalDebugComponent->m_showFPS) {
        #ifdef CE_BACKEND_PLAYDATE
            CE_GetPlaydateAPI()->system->drawFPS(0,0);
        #endif
    }
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION

#endif // CE_DEBUG_BUILD
