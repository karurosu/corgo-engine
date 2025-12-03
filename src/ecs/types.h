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
#define CE_OK 0 // Generic success code
#define CE_ERROR -1 // Generic error code

//// Id types

// Id is a 32-bit unsigned integer that identifies a unique object in CC ECS, either a component, entity or relationship.
// It is a bitset that can store different things depending on some data.
// Bits 32-29 (4 bits): Reference Type Component and flags.
// - Bit 32: Is Entity Reference (0) or Component Reference (1).
// - Bit 31: If Entity Reference, Direct Reference (0) or Relationship Reference (1).
// - Bits 30-29: Reserved for future use.
// Bits 28-23 (6 bits):
// - For entity reference: Generation count (0-63).
// - For Component Reference: Unused.
// Bits 22-17 (6 bits):
// - For Entity Reference: Relationship type in decimal (0-63). Only for relationship references, ignored otherwise.
// - For Component Reference: Component type in decimal (0-63).
// Bits 16-1 (16 bits): Unique id within the type (0-65535).
typedef uint32_t CE_Id;

// Component Type Id is a 8-bit unsigned integer that identifies a unique component type.
typedef uint8_t CE_TypeId;

//// CE_Id kind
// Different kinds of Ids.
typedef enum CE_IdKind {
    CE_ID_ENTITY_REFERENCE = 0,
    CE_ID_COMPONENT_REFERENCE = 1,
    CE_ID_ENTITY_RELATIONSHIP = 2,
    CE_ID_INVALID_KIND = 3
} CE_IdKind;

// Invalid entity id constant
#define CE_INVALID_ID 0

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

//// Inline flag tests
static inline bool CE_Id_isComponent(IN CE_Id id) { return (id & CE_ID_FLAG_COMPONENT) != 0; }
static inline bool CE_Id_isRelationship(IN CE_Id id) { return (!CE_Id_isComponent(id)) && ((id & CE_ID_FLAG_RELATIONSHIP) != 0); }
static inline bool CE_Id_isEntity(IN CE_Id id) { return (id & CE_ID_FLAG_COMPONENT) == 0; }

static inline CE_IdKind CE_Id_getKind(IN CE_Id id) {
    if (id == CE_INVALID_ID) return CE_ID_INVALID_KIND;
    if (CE_Id_isComponent(id)) return CE_ID_COMPONENT_REFERENCE;
    if (CE_Id_isRelationship(id)) return CE_ID_ENTITY_RELATIONSHIP;
    return CE_ID_ENTITY_REFERENCE;
}

//// Inline extractors
static inline uint32_t CE_Id_getUniqueId(IN CE_Id id) { return (id >> CE_ID_SHIFT_UNIQUE) & CE_ID_MASK_UNIQUE; }

static inline uint32_t CE_Id_getGeneration(IN CE_Id id) {
    if (!CE_Id_isEntity(id)) return CE_INVALID_ID;
    return (id >> CE_ID_SHIFT_GENERATION) & CE_ID_MASK_GENERATION;
}

static inline CE_TypeId CE_Id_getComponentTypeId(IN CE_Id id) {
    if (!CE_Id_isComponent(id)) return (CE_TypeId)CE_INVALID_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE6);
}

static inline CE_TypeId CE_Id_getRelationshipTypeId(IN CE_Id id) {
    if (!CE_Id_isRelationship(id)) return (CE_TypeId)CE_INVALID_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE6);
}

//// Mutators and utilities (implemented in core/types.c)
int CE_Id_setFlags(OUT CE_Id* id, CE_IdKind kind);
int CE_Id_setUniqueId(OUT CE_Id* id, uint32_t uniqueId);
int CE_Id_setGeneration(OUT CE_Id* id, uint32_t generation);
int CE_Id_setComponentTypeId(OUT CE_Id* id, CE_TypeId typeId);
int CE_Id_setRelationshipTypeId(OUT CE_Id* id, CE_TypeId typeId);

int CE_Id_make(CE_IdKind kind, CE_TypeId typeId, uint32_t generation, uint32_t uniqueId, OUT CE_Id* out);
bool CE_Id_compare(IN CE_Id a, IN CE_Id b);

//// Component types

// Maximum number of component types
// Currently limited to 64 so we can use a 64-bit bitmask to track which components an entity has.
#define CE_MAX_COMPONENT_TYPES 64

//// Internal data
typedef struct CE_ComponentStaticData {
    bool isValid;
    CE_TypeId type;
    uint32_t uid;
    uint64_t bitmask;
} CE_ComponentStaticData;

#endif // CORGO_ECS_TYPES_H