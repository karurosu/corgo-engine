//
//  engine/core/debug.c
//  Engine debugging tools.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifdef CE_DEBUG_BUILD

#include "engine/corgo.h"

void CE_Engine_SetGlobalDebug(IN CE_ECS_Context* context, IN bool enabled)
{
    CE_ECS_AccessGlobalComponent(context, CE_CORE_GLOBAL_DEBUG_COMPONENT)->m_enabled = enabled;
}

void CE_Engine_ShowFPSCounter(IN CE_ECS_Context* context, IN bool show)
{
    CE_Core_GlobalDebugComponent* globalDebug = CE_ECS_AccessGlobalComponent(context, CE_CORE_GLOBAL_DEBUG_COMPONENT);
    globalDebug->m_showFPS = show;
    if (show) {
        globalDebug->m_enabled = true;
    }
}

#endif // CE_DEBUG_BUILD
