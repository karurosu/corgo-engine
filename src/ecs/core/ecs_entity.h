//
//  ecs/core/ecs_entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ECS_ENTITY_H
#define CORGO_ECS_CORE_ECS_ENTITY_H

#include "ecs/types.h"

////////////////////////////////////
/// Creation Functions
////////////////////////////////////

/**
 * @brief Create a new entity in the ECS.
 * 
 * Allocates a new entity and returns the new Id.
 * The entity initially has no components attached.
 * The returned entity ID is valid and can be used for further ECS operations.
 * 
 * @param[in,out] context The ECS context.
 * @param[out] outId Pointer to receive the ID of the newly created entity.
 * @param[out] errorCode Optional error code if creation fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_ECS_CreateEntity(INOUT CE_ECS_Context* context, OUT CE_Id* outId, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Destroy an entity and remove all its components.
 * 
 * Removes the entity from the ECS and releases all associated component data.
 * Any components attached to the entity will be destroyed.
 * The entity ID becomes invalid after this call.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to destroy.
 * @param[out] errorCode Optional error code if destruction fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., entity not found).
 */
CE_Result CE_ECS_DestroyEntity(INOUT CE_ECS_Context* context, IN CE_Id entity, OUT_OPT CE_ERROR_CODE* errorCode);

////////////////////////////////////
// Entity Metadata
////////////////////////////////////

/**
 * @brief Check if an entity is valid and exists in the ECS.
 * 
 * Validates that the entity ID refers to an active entity in the context.
 * An entity is valid if it exists and has not been destroyed.
 * Lookup is very fast but it won't indicate why an entity is invalid.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The entity ID to validate.
 * 
 * @return true if the entity is valid and active, false otherwise.
 */
bool CE_Entity_IsValid(INOUT CE_ECS_Context* context, IN CE_Id entity);

/**
 * @brief Get the number of components attached to an entity.
 * 
 * Returns the total count of component instances currently attached to
 * the specified entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The entity ID to query.
 * 
 * @return The number of components attached to the entity, or 0 if invalid.
 */
size_t CE_Entity_GetComponentCount(INOUT CE_ECS_Context* context, IN CE_Id entity);

/**
 * @brief Get the number of relationships associated with an entity.
 * 
 * Returns the total count of relationships in this entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The entity ID to query.
 * 
 * @return The number of relationships associated with the entity, or 0 if invalid.
 */
size_t CE_Entity_GetRelationshipCount(INOUT CE_ECS_Context* context, IN CE_Id entity);

/**
 * @brief Check if an entity has at least one component of the specified type attached.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The entity ID to query.
 * @param[in] componentType The component type to check for.
 * 
 * @return true if the entity has the specified component type, false otherwise.
 */
bool CE_Entity_HasComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType);

/**
 * @brief Check if an entity has at least one relationship of the specified type attached.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The entity ID to query.
 * @param[in] relationshipType The relationship type to check for.
 * 
 * @return true if the entity has the specified relationship type, false otherwise.
 */
bool CE_Entity_HasRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType);

#endif // CORGO_ECS_CORE_ECS_ENTITY_H
