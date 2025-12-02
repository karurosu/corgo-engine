//
//  ecs/core/types.h
//  Main include file for ECS core types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//


#ifndef CORGO_ECS_TYPES_H
#define CORGO_ECS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

//// Shared includes
#define CC_NO_SHORT_NAMES
#include "cc.h"

//// Generic macros
#define IN  // Marks input parameters
#define OUT // Marks output parameters

//// Basic types

// Entity Id is a 32-bit unsigned integer that identifies a unique entity in the ECS.
typedef uint32_t CE_EntityId;

// Component Id is a 32-bit unsigned integer that identifies a unique component instance.
typedef uint32_t CE_ComponentId;

// Component Type Id is a 8-bit unsigned integer that identifies a unique component type.
typedef uint8_t CE_ComponentTypeId;

//// Constants

// Maximum number of component types
// Currently limited to 64 so we can use a 64-bit bitmask to track which components an entity has.
#define CE_MAX_COMPONENT_TYPES 64

// Invalid entity id constant
#define CE_INVALID_ENTITY 0

// Invalid component id constant
#define CE_INVALID_COMPONENT 0x0

//// Internal data
typedef struct CE_ComponentStaticData {
    bool isValid;
    CE_ComponentTypeId type;
    uint32_t uid;
    uint64_t bitmask;
} CE_ComponentStaticData;

#endif // CORGO_ECS_TYPES_H