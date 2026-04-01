//
//  ecs/core/components/debug_component.c
//  Simple component used for debugging purposes.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

#ifdef CE_DEBUG_BUILD

CE_DEFINE_COMPONENT_INIT(CE_CORE_DEBUG_COMPONENT)
{
    component->m_enabled = false;
#ifdef CE_CORE_TEST_MODE
    component->m_flagSystemTriggered = false;
    component->m_testValue = 0;
    component->m_testValue2 = 0;
    component->m_ticked_display = false;
    component->m_ticked_half = false;
    component->m_ticked_second = false;
    component->m_ticked_rel = false;
    component->m_tickedDebugSystem = false;
#endif
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_CORE_DEBUG_COMPONENT)
{
    // No dynamic resources to clean up for CE_Core_DebugComponent
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_CORE_GLOBAL_DEBUG_COMPONENT)
{
    component->m_enabled = false;
    component->m_showFPS = false;
#ifdef CE_CORE_TEST_MODE
    component->m_testValue = 0;
    component->m_tickedGlobalSystem = false;
    component->m_tickedDebugSystem = false;
    component->m_tickedComponentDebugSystem = false;
#endif
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_CORE_GLOBAL_DEBUG_COMPONENT)
{
    // No dynamic resources to clean up for CE_Core_GlobalDebugComponent
    return CE_OK;
}

#endif // CE_DEBUG_BUILD
