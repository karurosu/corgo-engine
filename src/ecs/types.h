//
//  ecs/core/types.h
//  Main include file for ECS core types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_TYPES_H
#define CORGO_ECS_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <cc.h>

// General helpers
#include "utils/helpers.h"
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

//// Component types

// Maximum number of component types
// Limited to 256 due to 8-bit type field.
#define CE_MAX_COMPONENT_TYPES 256

//// Exposed structures
typedef struct CE_ECS_ComponentStaticData CE_ECS_ComponentStaticData;
typedef struct CE_ECS_Context CE_ECS_Context;

// Adding this for convenience
#include "core/config.h"
#include "utils/bitset.h"

#endif // CORGO_ECS_TYPES_H