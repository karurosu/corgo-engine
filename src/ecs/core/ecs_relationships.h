//
//  ecs/core/ecs_relationships.h
//  Live Relationships manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ECS_RELATIONSHIPS_H
#define CORGO_ECS_CORE_ECS_RELATIONSHIPS_H

#include "ecs/types.h"

/////////////////////////////////////
// Relationship data access functions
/////////////////////////////////////

/**
 * @brief Add a relationship from one entity to another.
 * 
 * Creates a relationship of the specified type from the source entity to the target entity.
 * Optionally, a reciprocal relationship can be created on the target entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the source entity.
 * @param[in] relationshipType The type ID of the relationship to add.
 * @param[in] targetEntity The ID of the target entity.
 * @param[in] reciprocalRelationshipType The type ID of the reciprocal relationship to add on the target entity, or CE_INVALID_TYPE_ID to skip.
 * @param[out] errorCode Optional error code if addition fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_AddRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, IN CE_TypeId reciprocalRelationshipType, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Remove a relationship from one entity to another.
 * 
 * Removes the specified relationship from the source entity to the target entity.
 * Optionally, removes the reciprocal relationship from the target entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the source entity.
 * @param[in] relationshipType The type ID of the relationship to remove.
 * @param[in] targetEntity The ID of the target entity.
 * @param[in] reciprocalRelationshipType The type ID of the reciprocal relationship to remove on the target entity, or CE_INVALID_TYPE_ID to skip.
 * @param[out] errorCode Optional error code if removal fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_RemoveRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, IN CE_TypeId reciprocalRelationshipType, OUT_OPT CE_ERROR_CODE* errorCode);


#endif // CORGO_ECS_CORE_ECS_RELATIONSHIPS_H