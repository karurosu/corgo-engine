//
//  ecs/core/types.h
//  Main include file for ECS core types.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_TYPES_H
#define CORGO_ECS_TYPES_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1 // Disable warnings with strcpy, strncpy, etc.
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Convenient Containers
#include <cc.h>

// General helpers
#include "utils/helpers.h"

typedef int CE_Result;
#include "utils/error.h"

//// Id types
/*
 * Id is a 32-bit unsigned integer that identifies a unique object in CC ECS, either a component, entity, or relationship.
 * New bit schema:
 * For all types:
 * - Bits 32-29 (4 bits): Id Type enum value (0-15)
 *   0 -> invalid, 1 -> entity reference, 2 -> relationship, 3 -> component, 4-15 reserved
 * For entity references and relationships:
 * - Bits 28-25 (4 bits): Generation count (0-15).
 * - Bits 24-17 (8 bits):
 *   - For relationships: Relationship type in decimal (0-255).
 *   - For entity reference: unused.
 * - Bits 16-1 (16 bits): Unique id within the type (0-65535).
 * For components:
 * - Bits 28-25 (4 bits): Unused.
 * - Bits 24-17 (8 bits): Component type (0-255).
 * - Bits 16-1 (16 bits): Unique id within the type (0-65535).
 */
typedef uint32_t CE_Id;

// Component Type Id is a 8-bit unsigned integer that identifies a unique component type.
typedef uint8_t CE_TypeId;

// Short Id type: 16-bit unsigned integer for local ids within an Id type.
// Used to track objects temporarily without the full 32-bit Id.
// Use carefully as it doesn't store generation or component type.
typedef uint16_t CE_ShortId;

//// CE_Id kind
// Different kinds of Ids stored in the top 4 bits.
typedef enum CE_IdKind {
    CE_ID_INVALID_KIND = 0,
    CE_ID_ENTITY_REFERENCE_KIND = 1,
    CE_ID_ENTITY_RELATIONSHIP_KIND = 2,
    CE_ID_COMPONENT_REFERENCE_KIND = 3,
    CE_ID_KIND_COUNT
} CE_IdKind;

// Invalid constants
#define CE_INVALID_ID 0
#define CE_INVALID_TYPE_ID 0xFF

// Container types
#define CE_Id_Vector cc_vec(CE_Id)
#define CE_Id_Set cc_set(CE_Id)
#define CE_TypeId_Vector cc_vec(CE_TypeId)

//// Component types

// Maximum number of component types
// Limited to 255 due to 8-bit type field (0-254 are valid types, 255 is reserved for invalid type)
#define CE_MAX_COMPONENT_TYPES 255
#define CE_MAX_RELATIONSHIP_TYPES 255
#define CE_MAX_SYSTEM_TYPES 255

// Maximum generation count for entities and relationships (4 bits: 0-15)
#define CE_MAX_GENERATION_COUNT 15

//// Exposed structures
typedef struct CE_ECS_ComponentStaticData CE_ECS_ComponentStaticData;
typedef struct CE_ECS_Context CE_ECS_Context;
typedef struct CE_ECS_SystemStaticData CE_ECS_SystemStaticData;

// Adding this for convenience
#include "ecs/config.h"
#include "utils/bitset.h"

#endif // CORGO_ECS_TYPES_H