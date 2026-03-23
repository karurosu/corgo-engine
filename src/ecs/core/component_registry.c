//
//  ecs/core/component_registry.c
//  Centralized component metadata implementation registry.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "../components.h"

// Helper to generate the empty component functions
#define CE_NO_STORAGE_COMPONENT_GEN(name) \
CE_Result name##_init(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component) { return CE_OK; } \
CE_Result name##_cleanup(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component) { return CE_OK; }

// Generate wrappers and description functions for all registered components in one TU.
#define CE_COMPONENT_DESC(name, uid, storage, initial_capacity, ...) CE_GENERATE_COMPONENT_IMP(name, uid, storage, initial_capacity, __VA_ARGS__)
CE_COMPONENT_DESC_CORE(CE_COMPONENT_DESC)
CE_COMPONENT_DESC_ENGINE(CE_COMPONENT_DESC)
#ifndef CE_CORE_TEST_MODE
CE_COMPONENT_DESC_GAME(CE_COMPONENT_DESC)
#endif
#undef CE_COMPONENT_DESC

#undef CE_NO_STORAGE_COMPONENT_GEN

