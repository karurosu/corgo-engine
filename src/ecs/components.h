//
//  ecs/components.h
//  Components.h is a header file that includes the list of all component types, including core, engine, and game-defined components.
//  Gameplay code should only use this
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_COMPONENTS_H
#define CORGO_ECS_COMPONENTS_H

#include "types.h"
#include "core/component.h"

//// Build component list
// Core ECS components
#include "core/core_components.h"
#ifndef CE_COMPONENT_DESC_CORE
#define CE_COMPONENT_DESC_CORE(X)
#endif

// Engine components
#include "../engine/components.h"
#ifndef CE_COMPONENT_DESC_ENGINE
#define CE_COMPONENT_DESC_ENGINE(X)
#endif

// Game components
#include "../game/components.h"
#ifndef CE_COMPONENT_DESC_GAME
#define CE_COMPONENT_DESC_GAME(X)
#endif

//// Generate component Types Enum
typedef enum CE_COMPONENT_TYPES_ENUM {
#define X(name, uid, storage, initial_capacity) name,
	CE_COMPONENT_DESC_CORE(X)
	CE_COMPONENT_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_COMPONENT_DESC_GAME(X)
#endif
#undef X
	CE_COMPONENT_TYPES_COUNT //Invalid component count
} CE_COMPONENT_TYPES;

// Sanity check: ensure we don't exceed the bitmask capacity.
_Static_assert(CE_COMPONENT_TYPES_COUNT <= CE_MAX_COMPONENT_TYPES, "Too many component types: exceeds MAX_COMPONENT_TYPES");

//// Declare component global functions
#define X(name, uuid, storage, initial_capacity) CE_DECLARE_COMPONENT(name, uuid, storage, initial_capacity)
	CE_COMPONENT_DESC_CORE(X)
	CE_COMPONENT_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_COMPONENT_DESC_GAME(X)
#endif
#undef X

#endif