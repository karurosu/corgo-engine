//
//  ecs/core/entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ENTITY_H
#define CORGO_ECS_CORE_ENTITY_H

#include "ecs/types.h"

////////////////////////////////////
// Live Entity manipulation functions
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

#endif // CORGO_ECS_CORE_ENTITY_H