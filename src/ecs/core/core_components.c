//
//  ecs/core/core_components.h
//  List of core ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

void CE_CORE_DEBUG_COMPONENT_init(OUT CE_Core_DebugComponent* component)
{
    component->enabled = false;
}

void CE_CORE_DEBUG_COMPONENT_cleanup(OUT CE_Core_DebugComponent* component)
{
    // No dynamic resources to clean up for CE_Core_DebugComponent
}

CE_GENERATE_COMPONENT_IMP(CE_CORE_DEBUG_COMPONENT)
