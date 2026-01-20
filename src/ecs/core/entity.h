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
// Component Manipulation
////////////////////////////////////

/**
 * @brief Add a component to an entity.
 * 
 * Creates a new component instance of the specified type and attaches it
 * to the given entity. The component is initialized with default values.
 * The returned component ID is valid only for the lifetime of the entity and should not be shared with other entities.
 * The component data pointer, if provided, will point to the newly created component's data and can be used to initialize or modify the component.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to add the component to.
 * @param[in] componentType The type ID of the component to add.
 * @param[out] componentId Pointer to receive the ID of the new component instance.
 * @param[out] componentData Optional pointer to receive the address of the component data.
 * @param[out] errorCode Optional error code if addition fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., entity already has this component type).
 */
CE_Result CE_Entity_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Remove a component from an entity.
 * 
 * Detaches the specified component from an entity and releases its data.
 * Any pointers to the component data become invalid after this call.
 * The component ID becomes invalid after this call but may be reused for future components so it should not be cached. 
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to remove the component from.
 * @param[in] componentId The ID of the component to remove.
 * @param[out] errorCode Optional error code if removal fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component not found).
 */
CE_Result CE_Entity_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode);

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

////////////////////////////////////
/// Component data access functions
////////////////////////////////////

/**
 * @brief Retrieve a component instance from an entity.
 * 
 * Looks up the component for the specified component ID on the given entity.
 * This access is very fast as it only involves a direct lookup and does not require iteration over all components.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity containing the component.
 * @param[in] componentId The ID of the component to retrieve.
 * @param[out] componentData Pointer to receive the address of the component data. Do not save or cache this pointer.
 * @param[out] errorCode Optional error code if retrieval fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component not found).
 */
CE_Result CE_Entity_GetComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Find the first component of a specific type on an entity.
 * 
 * Searches for the first component matching the given type ID on the entity.
 * If the entity has multiple components of the same type, only the first is returned.
 * The order of the components is not guaranteed, use this if you are sure there is only one component of this type on the entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the component to find.
 * @param[out] componentId Pointer to receive the ID of the found component.
 * @param[out] componentData Pointer to receive the address of the component data. Do not save or cache this pointer.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component type not found).
 */
CE_Result CE_Entity_FindFirstComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id* componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
    
/**
 * @brief Find all components of a specific type on an entity.
 * 
 * Searches for all components matching the given type ID on the entity
 * and returns their IDs in the provided buffer.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the components to find.
 * @param[out] results Buffer to receive an array of component IDs.
 * @param[in] bufsize Maximum number of component IDs to return.
 * @param[out] resultCount The actual number of components found.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 * 
 * @note If the number of components exceeds bufsize, only bufsize results are returned
 *       and resultCount will contain the actual count found.
 */
CE_Result CE_Entity_FindAllComponents(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Retrieve a cc_set reference to all components of an entity.
 * 
 * The returned container is the raw data, so it should not be altered unless you know what you are doing.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the components to find.
 * @param[out] components Pointer to receive the set of component IDs. Do not modify the returned set.
 * @param[out] errorCode Optional error code if retrieval fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_GetAllComponentsIter(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id_Set **components, OUT_OPT CE_ERROR_CODE* errorCode);

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

#endif // CORGO_ECS_CORE_ENTITY_H
