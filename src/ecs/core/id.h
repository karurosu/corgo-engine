//
//  ecs/core/id.h
//  Utility functions for manipulating ids
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_ID_H
#define CORGO_ECS_CORE_ID_H

#include "ecs/types.h"

//// Bit layout helpers
// Field shifts and masks (0-indexed from LSB)
#define CE_ID_SHIFT_KIND          28
#define CE_ID_MASK_KIND           0xFu    // 4 bits
#define CE_ID_SHIFT_GENERATION    24
#define CE_ID_MASK_GENERATION     0xFu    // 4 bits
#define CE_ID_SHIFT_TYPE          16
#define CE_ID_MASK_TYPE8          0xFFu   // 8 bits
#define CE_ID_SHIFT_UNIQUE        0
#define CE_ID_MASK_UNIQUE         0xFFFFu // 16 bits

//// Inline kind tests
static inline CE_IdKind CE_Id_getKind(IN CE_Id id) {
    if (id == CE_INVALID_ID) return CE_ID_INVALID_KIND;
    return (CE_IdKind)(((id >> CE_ID_SHIFT_KIND) & CE_ID_MASK_KIND));
}

static inline bool CE_Id_isComponent(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_COMPONENT_REFERENCE_KIND; }
static inline bool CE_Id_isRelationship(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_ENTITY_RELATIONSHIP_KIND; }
static inline bool CE_Id_isEntity(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_ENTITY_REFERENCE_KIND; }

//// Inline extractors
static inline uint16_t CE_Id_getUniqueId(IN CE_Id id) { return (id >> CE_ID_SHIFT_UNIQUE) & CE_ID_MASK_UNIQUE; }

static inline uint8_t CE_Id_getGeneration(IN CE_Id id) {
    if (CE_Id_isComponent(id)) return 0;
    return (id >> CE_ID_SHIFT_GENERATION) & CE_ID_MASK_GENERATION;
}

static inline CE_TypeId CE_Id_getComponentTypeId(IN CE_Id id) {
    if (!CE_Id_isComponent(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE8);
}

static inline CE_TypeId CE_Id_getRelationshipTypeId(IN CE_Id id) {
    if (!CE_Id_isRelationship(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE8);
}

//// Mutators and utilities
CE_Result CE_Id_setKind(INOUT CE_Id* id, CE_IdKind kind);
CE_Result CE_Id_setUniqueId(INOUT CE_Id* id, uint16_t uniqueId);
CE_Result CE_Id_setGeneration(INOUT CE_Id* id, uint8_t generation);
CE_Result CE_Id_setComponentTypeId(INOUT CE_Id* id, CE_TypeId typeId);
CE_Result CE_Id_setRelationshipTypeId(INOUT CE_Id* id, CE_TypeId typeId);

CE_Result CE_Id_make(IN CE_IdKind kind, IN CE_TypeId typeId, IN uint8_t generation, IN uint16_t uniqueId, OUT CE_Id* out);
CE_Id CE_Id_relationshipToEntityReference(IN CE_Id relationshipId);

#endif