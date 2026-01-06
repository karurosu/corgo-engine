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
 * Id is a 32-bit unsigned integer that identifies a unique object in CC ECS, either a component, entity or relationship.
 * It is a bitset that can store different things depending on some data.
 * Bits 32-29 (4 bits): Reference Type Component and flags.
 * - Bit 32: Is Entity Reference (0) or Component Reference (1).
 * - Bit 31: If Entity Reference, Direct Reference (0) or Relationship Reference (1).
 * - Bits 30-29: Reserved for future use.
 * Bits 28-23 (6 bits):
 * - For entity reference: Generation count (0-63).
 * - For Component Reference: Unused.
 * Bits 22-17 (6 bits):
 * - For Entity Reference: Relationship type in decimal (0-63). Only for relationship references, ignored otherwise.
 * - For Component Reference: Component type in decimal (0-63).
 * Bits 16-1 (16 bits): Unique id within the type (0-65535).
 */
typedef uint32_t CE_Id;

// Component Type Id is a 8-bit unsigned integer that identifies a unique component type.
typedef uint8_t CE_TypeId;

//// CE_Id kind
// Different kinds of Ids.
typedef enum CE_IdKind {
    CE_ID_ENTITY_REFERENCE_KIND = 0,
    CE_ID_COMPONENT_REFERENCE_KIND = 1,
    CE_ID_ENTITY_RELATIONSHIP_KIND = 2,
    CE_ID_INVALID_KIND = 3
} CE_IdKind;

// Invalid constants
#define CE_INVALID_ID 0
#define CE_INVALID_TYPE_ID 0xFF

//// Bit layout helpers
// Flags (bits 31 and 30 in 0-indexed from MSB, described as 32 and 31 in comments)
#define CE_ID_FLAG_COMPONENT      (1u << 31)
#define CE_ID_FLAG_RELATIONSHIP   (1u << 30)

// Field shifts and masks
#define CE_ID_SHIFT_GENERATION    23
#define CE_ID_MASK_GENERATION     0x3Fu   // 6 bits
#define CE_ID_SHIFT_TYPE          17
#define CE_ID_MASK_TYPE6          0x3Fu   // 6 bits
#define CE_ID_SHIFT_UNIQUE        0
#define CE_ID_MASK_UNIQUE         0xFFFFu // 16 bits

// Container types
#define CE_Id_Vector cc_vec(CE_Id)

//// Component types

// Maximum number of component types
// Currently limited to 64 due to id size.
#define CE_MAX_COMPONENT_TYPES 64

//// Exposed structures
typedef struct CE_ECS_ComponentStaticData CE_ECS_ComponentStaticData;
typedef struct CE_ECS_Context CE_ECS_Context;

// Adding this for convenience
#include "core/config.h"
#include "utils/bitset.h"

#endif // CORGO_ECS_TYPES_H