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
#include "engine/core/memory.h"
#include "core/id.h"
#include "components.h"
#include "systems.h"
#include "relationships.h"
#include "core/storage.h"
#include "core/context.h"

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

// Include the separate headers for each module
#include "core/ecs_component.h"
#include "core/ecs_entity.h"
#include "core/ecs_relationships.h"

#endif // CORGO_ECS_ECS_H