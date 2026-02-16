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

// Getters

/**
 * @brief Get the kind of the given id.
 * 
 * @param[in] id The id to query.
 * @return The kind of the id.
 * Possible values:
 * - CE_ID_INVALID_KIND
 * - CE_ID_ENTITY_REFERENCE_KIND
 * - CE_ID_ENTITY_RELATIONSHIP_KIND
 * - CE_ID_COMPONENT_REFERENCE_KIND
 */
static inline CE_IdKind CE_Id_getKind(IN CE_Id id) {
    if (id == CE_INVALID_ID) return CE_ID_INVALID_KIND;
    return (CE_IdKind)(((id >> CE_ID_SHIFT_KIND) & CE_ID_MASK_KIND));
}

/**
 * @brief Check if the given id is a component reference.
 * 
 * @param[in] id The id to check.
 * @return true if the id is a component reference, false otherwise.
 */
static inline bool CE_Id_isComponent(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_COMPONENT_REFERENCE_KIND; }

/**
 * @brief Check if the given id is a relationship reference.
 * 
 * @param[in] id The id to check.
 * @return true if the id is a relationship reference, false otherwise.
 */
static inline bool CE_Id_isRelationship(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_ENTITY_RELATIONSHIP_KIND; }

/**
 * @brief Check if the given id is an entity reference.
 * 
 * @param[in] id The id to check.
 * @return true if the id is an entity reference, false otherwise.
 */
static inline bool CE_Id_isEntity(IN CE_Id id) { return CE_Id_getKind(id) == CE_ID_ENTITY_REFERENCE_KIND; }

/**
 * @brief Get the unique id part of the given id.
 * 
 * @param[in] id The id to query.
 * @return The unique id part (16 bits).
 */
static inline uint16_t CE_Id_getUniqueId(IN CE_Id id) { return (id >> CE_ID_SHIFT_UNIQUE) & CE_ID_MASK_UNIQUE; }


/**
 * @brief Get the generation part of the given id.
 * 
 * @param[in] id The id to query.
 * @return The generation part (4 bits) or CE_INVALID_ID for component references.
 */
static inline uint8_t CE_Id_getGeneration(IN CE_Id id) {
    if (CE_Id_isComponent(id)) return 0;
    return (id >> CE_ID_SHIFT_GENERATION) & CE_ID_MASK_GENERATION;
}

 /**
 * @brief Get the component type id part of the given id.
 * 
 * @param[in] id The id to query.
 * @return The component type id part (8 bits) or CE_INVALID_TYPE_ID for non-component references.
 */
static inline CE_TypeId CE_Id_getComponentTypeId(IN CE_Id id) {
    if (!CE_Id_isComponent(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE8);
}

/**
 * @brief Get the relationship type id part of the given id.
 * 
 * @param[in] id The id to query.
 * @return The relationship type id part (8 bits) or CE_INVALID_TYPE_ID for non-relationship references.
 */
static inline CE_TypeId CE_Id_getRelationshipTypeId(IN CE_Id id) {
    if (!CE_Id_isRelationship(id)) return (CE_TypeId)CE_INVALID_TYPE_ID;
    return (CE_TypeId)((id >> CE_ID_SHIFT_TYPE) & CE_ID_MASK_TYPE8);
}

// Setters

/**
 * @brief Set the kind of the given id.
 * Changing the kind will not modify other fields, they must be manually corrected or the id will be malformed.
 * 
 * @param[in,out] id Pointer to the id to modify.
 * @param[in] kind The new kind to set.
 * @return CE_OK on success, CE_ERROR on Invalid pointer or invalid kind.
 */
CE_Result CE_Id_setKind(INOUT CE_Id* id, CE_IdKind kind);

/**
 * @brief Set the unique id part of the given id.
 * 
 * @param[in,out] id Pointer to the id to modify.
 * @param[in] uniqueId The new unique id to set.
 * @return CE_OK on success, CE_ERROR on Invalid pointer or unique id out of range (0-65535).
 */
CE_Result CE_Id_setUniqueId(INOUT CE_Id* id, uint16_t uniqueId);

/**
 * @brief Set the generation part of the given id.
 * 
 * @param[in,out] id Pointer to the id to modify.
 * @param[in] generation The new generation to set, must be smaller than CE_MAX_GENERATION_COUNT.
 * @return CE_OK on success, CE_ERROR on Invalid pointer, invalid generation or invalid type.
 */
CE_Result CE_Id_setGeneration(INOUT CE_Id* id, uint8_t generation);

/**
 * @brief Set the component type id part of the given id.
 * 
 * @param[in,out] id Pointer to the id to modify.
 * @param[in] typeId The new component type id to set.
 * @return CE_OK on success, CE_ERROR on Invalid pointer, invalid id kind.
 */
CE_Result CE_Id_setComponentTypeId(INOUT CE_Id* id, CE_TypeId typeId);

/**
 * @brief Set the relationship type id part of the given id.
 * 
 * @param[in,out] id Pointer to the id to modify.
 * @param[in] typeId The new relationship type id to set.
 * @return CE_OK on success, CE_ERROR on Invalid pointer, invalid id kind.
 */
CE_Result CE_Id_setRelationshipTypeId(INOUT CE_Id* id, CE_TypeId typeId);

/**
 * @brief Construct a new CE_Id from its components. 
 * Type is set to 0 for entity references.
 * 
 * @param[in] kind The kind of id to create.
 * @param[in] typeId The component/relationship type id (ignored for entity references).
 * @param[in] generation The generation (ignored for component references).
 * @param[in] uniqueId The unique id.
 * @param[out] out Pointer to store the constructed id, CE_INVALID_ID on failure.
 * 
 * @return CE_OK on success, CE_ERROR on invalid parameters:
 * - out is NULL
 * - kind is invalid
 * - generation is out of range for entity/relationship references
 * - typeId is out of range for component/relationship references
 */
CE_Result CE_Id_make(IN CE_IdKind kind, IN CE_TypeId typeId, IN uint8_t generation, IN uint16_t uniqueId, OUT CE_Id* out);

/**
 * @brief Convert a relationship id to its corresponding entity reference id.
 * This method ensures that the resulting id is directly usable as an entity reference, including comparisons.
 * 
 * @param[in] relationshipId The relationship id to convert.
 * @return The corresponding entity reference id, or CE_INVALID_ID if the input id is not a relationship.
 */
CE_Id CE_Id_relationshipToEntityReference(IN CE_Id relationshipId);

/**
 * @brief Check if two ids refer to the same entity (ignoring kind).
 * This usually means:
 * - Both ids are entity references with same unique id and generation.
 * - One id is an entity reference and the other is a relationship to that entity with same unique id and generation.
 * - If any of the ids is invalid or a component reference, returns false.
 * 
 * @param[in] id1 The first id to compare.
 * @param[in] id2 The second id to compare.
 * @return true if both ids refer to the same entity, false otherwise.
 */
bool CE_Id_entityMatches(IN CE_Id id1, IN CE_Id id2);

#endif