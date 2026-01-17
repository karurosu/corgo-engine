//
//  ecs/ecs.h
//  Primary ECS include.
//  This is the public API for Corgo ECS.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_ECS_H
#define CORGO_ECS_ECS_H

#include "types.h"
#include "engine/core/platform.h"
#include "core/id.h"
#include "components.h"
#include "systems.h"
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

////////////////////////////////////
/// Entity data access functions
/// Included here for public use
////////////////////////////////////

#include "core/entity.h"

////////////////////////////////////
/// Internal ECS functions, exposed here because user code may need to call them
/// But regular should not use it directly
////////////////////////////////////

CE_Result CE_ECS_GetComponentForSystem(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, const IN CE_ECS_SystemStaticData *system, OUT CE_Id* componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

#endif // CORGO_ECS_ECS_H