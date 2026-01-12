//
//  ecs/core/core_components.h
//  List of core ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_COMPONENTS_H
#define CORGO_ECS_CORE_COMPONENTS_H

#include "component.h"

typedef struct CE_Core_DebugComponent {
    bool m_enabled;
    uint8_t m_debugValue;
} CE_Core_DebugComponent;

#define CE_COMPONENT_DESC_CORE(X) \
	X(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent, CE_DEFAULT_COMPONENT_CAPACITY)

#endif // CORGO_ECS_CORE_COMPONENTS_H