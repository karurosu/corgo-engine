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
 * @param[out] errorCode Optional error code if addition fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_AddRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT_OPT CE_ERROR_CODE* errorCode);

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
 * @param[out] errorCode Optional error code if removal fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_RemoveRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT_OPT CE_ERROR_CODE* errorCode);

/////////////////////////////////
// Relationship query functions
/////////////////////////////////

/**
 * @brief Find the first relationship of a specific type on an entity.
 * 
 * Searches for the first relationship matching the given type ID on the entity.
 * If the entity has multiple relationships of the same type, only the first is returned.
 * The order of the relationships is not guaranteed, use this if you are sure there is only one relationship of this type on the entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] relationshipType The type ID of the relationship to find.
 * @param[out] relationshipId Pointer to receive the ID of the found relationship.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., relationship type not found).
 */
CE_Result CE_Entity_FindFirstRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, OUT CE_Id* relationshipId, OUT_OPT CE_ERROR_CODE* errorCode);
    
/**
 * @brief Find all relationships of a specific type on an entity.
 * 
 * Searches for all relationships matching the given type ID on the entity
 * and returns their IDs in the provided buffer.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] relationshipType The type ID of the relationships to find.
 * @param[out] results Buffer to receive an array of relationship IDs.
 * @param[in] bufsize Maximum number of relationship IDs to return.
 * @param[out] resultCount The actual number of relationships found.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 * 
 * @note If the number of relationships exceeds bufsize, only bufsize results are returned
 *       and resultCount will contain the actual count found.
 */
CE_Result CE_Entity_FindAllRelationships(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Check if an entity has a specific relationship with a target entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to check.
 * @param[in] relationshipType The type ID of the relationship to check.
 * @param[in] targetEntity The ID of the target entity.
 * @param[out] exists Pointer to a boolean that will be set to true if the relationship exists, false otherwise.    
 * @param[out] errorCode Optional error code if check fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_HasSpecificRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT bool* exists, OUT_OPT CE_ERROR_CODE* errorCode);

////////////////////////////////////
/// Internal ECS functions, exposed here because user code may need to call them
/// But regular should not use it directly
////////////////////////////////////

CE_Result CE_ECS_GetRelationshipForSystem(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, const IN CE_ECS_SystemStaticData *system, OUT CE_Id* targetId, OUT_OPT CE_ERROR_CODE* errorCode);


#endif // CORGO_ECS_CORE_ECS_RELATIONSHIPS_H