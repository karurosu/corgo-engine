//
//  ecs/systems.h
//  Main include file for all systems definitions.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_SYSTEMS_H
#define CORGO_ECS_SYSTEMS_H

#include "types.h"
#include "core/system.h"

//// Build system list
// Core ECS systems
#include "core/core_systems.h"
#ifndef CE_SYSTEM_DESC_CORE
#define CE_SYSTEM_DESC_CORE(X)
#endif

// Engine systems
//#include "../engine/systems.h"
#ifndef CE_SYSTEM_DESC_ENGINE
#define CE_SYSTEM_DESC_ENGINE(X)
#endif

// Game systems
//#include "../game/systems.h"
#ifndef CE_SYSTEM_DESC_GAME
#define CE_SYSTEM_DESC_GAME(X)
#endif

//// Generate system Types Enum
typedef enum CE_SYSTEM_TYPES_ENUM {
#define X(name, uid, storage, initial_capacity, ...) name,
	CE_SYSTEM_DESC_CORE(X)
	CE_SYSTEM_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_SYSTEM_DESC_GAME(X)
#endif
#undef X
	CE_SYSTEM_TYPES_COUNT //Invalid system count
} CE_SYSTEM_TYPES;

// Sanity check: ensure we don't exceed the bitmask capacity.
_Static_assert(CE_SYSTEM_TYPES_COUNT <= CE_MAX_SYSTEM_TYPES, "Too many system types: exceeds MAX_SYSTEM_TYPES");

//// Declare system global functions
#define X(name, run_order, run_phase, run_frequency, ...) CE_DECLARE_SYSTEM(name, run_order, run_phase, run_frequency, __VA_ARGS__)
	CE_SYSTEM_DESC_CORE(X)
	CE_SYSTEM_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_SYSTEM_DESC_GAME(X)
#endif
#undef X

#endif // CORGO_ECS_SYSTEMS_H