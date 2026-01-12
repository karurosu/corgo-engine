//
//  ecs/ecs.h
//  Primary ECS include (single entry point for Corgo ECS).
//  This is the public API for Corgo ECS.
//  Copyright (c) 2025 Carlos Camacho.
//

#ifndef CORGO_ECS_ECS_H
#define CORGO_ECS_ECS_H

#include "types.h"
#include "core/id.h"
#include "components.h"
#include "core/storage.h"

////////////////////////////////////
/// Main ECS Functions
////////////////////////////////////

/**
 * @brief Initialize the ECS context.
 * 
 * Sets up the ECS subsystem, registers all component types, and prepares
 * the context for entity and component operations. This must be called before
 * any other ECS operations.
 * 
 * @param[in,out] context The ECS context to initialize. Must be allocated by caller.
 * @param[out] errorCode Optional error code if initialization fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE *errorCode);

/**
 * @brief Clean up and release all ECS resources.
 * 
 * Destroys all entities, releases component storage, and deallocates
 * internal structures. Call this when the ECS is no longer needed.
 * Any remaining entities or components will be destroyed, and all pointers will be invalid.
 * 
 * @param[in,out] context The ECS context to clean up.
 * @param[out] errorCode Optional error code if cleanup fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_ECS_Cleanup(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Update the ECS and execute all registered systems.
 * 
 * Advances the ECS simulation by the given delta time, running all
 * registered systems to process entities and components.
 * 
 * @param[in,out] context The ECS context to update.
 * @param[in] deltaTime Time elapsed since the last tick, in seconds.
 * @param[out] errorCode Optional error code if update fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_ECS_Tick(INOUT CE_ECS_Context* context, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);

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
CE_Result CE_ECS_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

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
CE_Result CE_ECS_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode);

////////////////////////////////////
/// Entity data access functions
/// Included here for public use
////////////////////////////////////

#include "core/entity.h"

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
CE_Result CE_ECS_GetComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

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
CE_Result CE_ECS_FindFirstComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id* componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
    
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
CE_Result CE_ECS_FindAllComponents(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode);

#endif // CORGO_ECS_ECS_H