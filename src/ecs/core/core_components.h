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
    uint8_t m_testValue2;
    bool m_ticked_display;
    bool m_ticked_half;
    bool m_ticked_second;
    bool m_ticked_rel;
#endif
} CE_Core_DebugComponent;

typedef struct CE_Core_GlobalDebugComponent {
    bool m_enabled;
#ifdef CE_CORE_TEST_MODE
    uint32_t m_testValue;
#endif
} CE_Core_GlobalDebugComponent;

// Core components uid range: 0-9

#define CE_COMPONENT_DESC_CORE(CE_COMPONENT_DESC) \
	CE_COMPONENT_DESC(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent, CE_DEFAULT_COMPONENT_CAPACITY)\
    CE_NS_COMPONENT_DESC(CE_CORE_NO_STORAGE_COMPONENT_TEST, 1)\


#define CE_GLOBAL_COMPONENT_DESC_CORE(CE_GLOBAL_COMPONENT_DESC) \
    CE_GLOBAL_COMPONENT_DESC(CE_CORE_GLOBAL_DEBUG_COMPONENT, CE_Core_GlobalDebugComponent)

#endif // CORGO_ECS_CORE_COMPONENTS_H