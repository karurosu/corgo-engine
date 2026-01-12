//
//  ecs/core/core_components.h
//  List of core ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

CE_DEFINE_COMPONENT_INIT(CE_CORE_DEBUG_COMPONENT)
{
    component->m_enabled = false;
    component->m_debugValue = 0;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_CORE_DEBUG_COMPONENT)
{
    // No dynamic resources to clean up for CE_Core_DebugComponent
    return CE_OK;
}

CE_GENERATE_COMPONENT_IMP(CE_CORE_DEBUG_COMPONENT)
