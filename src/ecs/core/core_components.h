//
//  ecs/core/core_components.h
//  List of core components for the ECS.
//  Not to be confused with components.h which is a helper with all component types
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_COMPONENTS_H
#define CORGO_ECS_CORE_COMPONENTS_H

#include "component.h"

typedef struct CE_Core_DebugComponent {
    bool m_enabled;
#ifdef CE_CORE_TEST_MODE
    uint8_t m_testValue;
    bool m_ticked_display;
    bool m_ticked_half;
    bool m_ticked_second;
#endif
} CE_Core_DebugComponent;

typedef struct CE_Core_GlobalDebugComponent {
    bool m_enabled;
#ifdef CE_CORE_TEST_MODE
    uint32_t m_testValue;
#endif
} CE_Core_GlobalDebugComponent;

#define CE_COMPONENT_DESC_CORE(X) \
	X(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent, CE_DEFAULT_COMPONENT_CAPACITY)

#define CE_GLOBAL_COMPONENT_DESC_CORE(X) \
    X(CE_CORE_GLOBAL_DEBUG_COMPONENT, CE_Core_GlobalDebugComponent)

#endif // CORGO_ECS_CORE_COMPONENTS_H