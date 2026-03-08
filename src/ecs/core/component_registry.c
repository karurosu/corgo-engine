//
//  ecs/core/component_registry.c
//  Centralized component metadata implementation registry.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "../components.h"

// Generate wrappers and description functions for all registered components in one TU.
#define X(name, uid, storage, initial_capacity) CE_GENERATE_COMPONENT_IMP(name)
CE_COMPONENT_DESC_CORE(X)
CE_COMPONENT_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
CE_COMPONENT_DESC_GAME(X)
#endif
#undef X
