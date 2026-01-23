//
//  ecs/core/core_systems.h
//  List of core ECS systems
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//
#ifndef CORGO_ECS_CORE_SYSTEMS_H
#define CORGO_ECS_CORE_SYSTEMS_H

#include "system.h"

////////////////////////////////////////////////////////////////////////
// Test Systems Definitions
////////////////////////////////////////////////////////////////////////

// Test dependecy, just the debug component
#define CE_CORE_TEST_SYSTEM_DEPENDENCIES \
    REQUIRE_COMPONENT(CE_CORE_DEBUG_COMPONENT, debugComponent)

// Test dependency with relationship
#define CE_CORE_TEST_SYSTEM_DEPENDENCIES_REL \
    REQUIRE_COMPONENT(CE_CORE_DEBUG_COMPONENT, debugComponent)\
    REQUIRE_RELATIONSHIP(CE_RELATIONSHIP_PARENT, relatedComponent)

// The systems CE_CORE_TEST_SYSTEM_* are for testing purposes, only included on test builds
#ifdef CE_CORE_TEST_MODE
#define CE_SYSTEM_TEST_CORE(X) \
	X(CE_CORE_TEST_SYSTEM_DISPLAY, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, CE_CORE_TEST_SYSTEM_DEPENDENCIES)\
    X(CE_CORE_TEST_SYSTEM_HALF_DISPLAY, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_EARLY, CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, CE_CORE_TEST_SYSTEM_DEPENDENCIES)\
    X(CE_CORE_TEST_SYSTEM_SECOND, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_LATE, CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND, CE_CORE_TEST_SYSTEM_DEPENDENCIES)\
    X(CE_CORE_TEST_SYSTEM_REL, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, CE_CORE_TEST_SYSTEM_DEPENDENCIES_REL)
#else 
#define CE_SYSTEM_TEST_CORE(X)
#endif // CE_CORE_TEST_MODE

////////////////////////////////////////////////////////////////////////
// Core Systems Definitions
////////////////////////////////////////////////////////////////////////

#define CE_CORE_DEBUG_SYSTEM_DEPENDENCIES \
    REQUIRE_COMPONENT(CE_CORE_DEBUG_COMPONENT, debugComponent)

#define CE_SYSTEM_DESC_CORE(X) \
    CE_SYSTEM_TEST_CORE(X)\
    X(CE_CORE_DEBUG_SYSTEM, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_LATE, CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)


#endif // CORGO_ECS_CORE_SYSTEMS_H