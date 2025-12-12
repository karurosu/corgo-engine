//
//  ecs/core/id.h
//  Utility functions for manipulating ids
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_ID_H
#define CORGO_ECS_CORE_ID_H

#include "ecs/types.h"

//// Inline flag tests
static inline bool CE_Id_isComponent(IN CE_Id id) { return (id != CE_INVALID_ID && (id & CE_ID_FLAG_COMPONENT) != 0); }
static inline bool CE_Id_isRelationship(IN CE_Id id) { return (!CE_Id_isComponent(id)) && ((id & CE_ID_FLAG_RELATIONSHIP) != 0); }
static inline bool CE_Id_isEntity(IN CE_Id id) { return (id != CE_INVALID_ID && (id & CE_ID_FLAG_COMPONENT) == 0); }

static inline CE_IdKind CE_Id_getKind(IN CE_Id id) {
    if (id == CE_INVALID_ID) return CE_ID_INVALID_KIND;
    if (CE_Id_isComponent(id)) return CE_ID_COMPONENT_REFERENCE_KIND;
    if (CE_Id_isRelationship(id)) return CE_ID_ENTITY_RELATIONSHIP_KIND;
    return CE_ID_ENTITY_REFERENCE_KIND;
}

//// Inline extractors
static inline uint32_t CE_Id_getUniqueId(IN CE_Id id) { return (id >> CE_ID_SHIFT_UNIQUE) & CE_ID_MASK_UNIQUE; }

static inline uint32_t CE_Id_getGeneration(IN CE_Id id) {
    if (!CE_Id_isEntity(id)) return 0;
    return (id >> CE_ID_SHIFT_GENERATION) & CE_ID_MASK_GENERATION;
}

static inline CE_TypeId CE_Id_getComponentTypeId(IN CE_Id id) {
    if (!CE_Id_isComponent(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE6);
}

static inline CE_TypeId CE_Id_getRelationshipTypeId(IN CE_Id id) {
    if (!CE_Id_isRelationship(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE6);
}

//// Mutators and utilities (implemented in core/types.c)
CE_Result CE_Id_setKind(INOUT CE_Id* id, CE_IdKind kind);
CE_Result CE_Id_setUniqueId(INOUT CE_Id* id, uint32_t uniqueId);
CE_Result CE_Id_setGeneration(INOUT CE_Id* id, uint32_t generation);
CE_Result CE_Id_setComponentTypeId(INOUT CE_Id* id, CE_TypeId typeId);
CE_Result CE_Id_setRelationshipTypeId(INOUT CE_Id* id, CE_TypeId typeId);

CE_Result CE_Id_make(IN CE_IdKind kind, IN CE_TypeId typeId, IN uint32_t generation, IN uint32_t uniqueId, OUT CE_Id* out);
bool CE_Id_compare(IN CE_Id a, IN CE_Id b);

#endif