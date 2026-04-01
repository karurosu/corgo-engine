//
//  engine/core/scene.h
//  Scene graph component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_SCENE_H
#define CORGO_ENGINE_CORE_SCENE_H

#include "ecs/types.h"

// Unique identifier for scenes
typedef const char * CE_SceneId;

// Signatures for create and run functions
typedef CE_Result (*CE_SceneRunFunction)(INOUT struct CE_ECS_Context* context, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);
typedef CE_Result (*CE_SceneCreateFunction)(INOUT struct CE_ECS_Context* context, void *dataComponent, OUT_OPT CE_ERROR_CODE* errorCode);

// Tracked states of a scene
typedef enum CE_SceneState
{
    CE_SCENE_STATE_UNLOADED,
    CE_SCENE_STATE_START_LOAD,
    CE_SCENE_STATE_LOADING,
    CE_SCENE_STATE_RUNNING,
    CE_SCENE_STATE_UNLOADING
} CE_SceneState;

// Runtime data for a scene
typedef struct CE_Scene
{
    CE_SceneId m_id;
    CE_TypeId m_scriptDataComponentType;
    CE_SceneRunFunction m_runFunction;
    CE_SceneCreateFunction m_createFunction;
} CE_Scene;

// Helper function to clear the scene
static inline void CE_Engine_Scene_Clear(IN CE_Scene* scene)
{
    scene->m_id = NULL;
    scene->m_scriptDataComponentType = CE_INVALID_TYPE_ID;
    scene->m_runFunction = NULL;
    scene->m_createFunction = NULL;
}

// Function used to populate the scene data
typedef CE_Result (*CE_LoadSceneDataFunction)(INOUT struct CE_ECS_Context* context, OUT CE_Scene* scene, OUT_OPT CE_ERROR_CODE* errorCode);

// Public API

/** 
 * @brief Macro: Get the root entity ID of the scene graph.
 * 
 * @param contextPtr[in] The ECS context.
 * @return The root entity ID.
 */
#define CE_Scene_GetRootId(contextPtr) \
    (CE_ECS_AccessGlobalComponent((contextPtr), CE_ENGINE_SCENE_GRAPH_COMPONENT)->m_rootEntityId)

/** 
 * @brief Adds a child entity to a parent entity in the scene graph.
 * Ensures caches are updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param parentId[in] The parent entity ID.
 * @param childId[in] The child entity ID.
 * @param move[in] Whether to move the child from its current parent if it already has one.
 * @param errorCode[out] The error code in case of failure.
 * 
 * @return CE_OK if the child was successfully added, CE_ERROR if it failed.
 */
CE_Result CE_Scene_AddChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, IN bool move, CE_ERROR_CODE* errorCode);

/**
 * @brief Removes a child entity from a parent entity in the scene graph.
 * Ensures caches are updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param parentId[in] The parent entity ID.
 * @param childId[in] The child entity ID.
 * @param errorCode[out] The error code in case of failure.
 * 
 * @return CE_OK if the child was successfully removed, CE_ERROR if it failed.
 */
CE_Result CE_Scene_RemoveChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, CE_ERROR_CODE* errorCode);

/**
 * @brief Gets the render node for a given entity ID from the scene graph cache.
 * Returns NULL if the entity does not have a render node in the cache.
 * 
 * @param context[in] The ECS context.
 * @param entityId[in] The entity ID to get the render node for.
 * 
 * @return The render node for the entity, or NULL if it does not exist in the cache.
 */
CE_SceneGraphRenderNode *CE_Scene_GetRenderNode(IN CE_ECS_Context* context, IN CE_Id entityId);

/**
 * @brief Requests the loading of a scene and replace the current one.
 * The current scene will be fully unloaded first.
 * This will overwrite any previous requests but will fail if the scene has started loading.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param loadFunction[in] The function that will populate the scene data, must be of signature CE_LoadSceneDataFunction.
 * @param errorCode[out] Optional pointer to receive an error code if the request fails.
 * 
 * @return CE_OK if the load request was successfully initiated, CE_ERROR if it failed.
 */
CE_Result CE_Scene_RequestLoad(INOUT struct CE_ECS_Context* context, IN CE_LoadSceneDataFunction loadFunction, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Requests the unloading of the current scene.
 * In general you don't want to call that unless you intend to manage the scene manually, as no scene will be loaded.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param errorCode[out] Optional pointer to receive an error code if the request fails.
 * 
 * @return CE_OK if the unload request was successfully initiated, CE_ERROR if it failed.
 */
CE_Result CE_Scene_RequestUnload(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Checks if a scene is currently running.
 * 
 * @param context[in,out] The ECS context to operate on.
 * 
 * @return true if a scene is currently running, false otherwise.
 */
bool CE_Scene_IsSceneRunning(INOUT struct CE_ECS_Context* context);

#define CE_SCENE_LOAD_FUNCTION(name) CE_PASTE(name, _LoadSceneDataFunction)
#define CE_SCENE_CREATE_FUNCTION(name)  CE_PASTE(name, _CreateFunction)
#define CE_SCENE_RUN_FUNCTION(name) CE_PASTE(name, _RunFunction)

/**
 * @brief Macro to define a scene create function with the correct signature.
 * @param sceneName[in] unique name for the scene
 */
#define CE_DECLARE_SCENE_CREATE_FUNCTION(sceneName) \
    CE_Result CE_SCENE_CREATE_FUNCTION(sceneName)(INOUT struct CE_ECS_Context* context, void *dataComponent, OUT_OPT CE_ERROR_CODE* errorCode)

/**
 * @brief Macro to define a scene create function with the correct signature.
 * @param sceneName[in] unique name for the scene
 */
#define CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(sceneName) \
    CE_Result CE_SCENE_LOAD_FUNCTION(sceneName)(INOUT struct CE_ECS_Context* context, OUT CE_Scene* scene, OUT_OPT CE_ERROR_CODE* errorCode)

/**
 * @brief Macro to define a scene run function with the correct signature.
 * @param sceneName[in] unique name for the scene
 */
#define CE_DECLARE_SCENE_RUN_FUNCTION(sceneName) \
    CE_Result CE_SCENE_RUN_FUNCTION(sceneName)(INOUT struct CE_ECS_Context* context, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)

/**
 * @brief Macro to define all scene functions (load data, create and run) with the correct signatures.
 * Optional, but saves some boilerplate when defining a new scene. You can still define the functions manually if you want to.
 * Run function is optional. 
 * @param sceneName[in] unique name for the scene
 */
#define CE_DECLARE_SCENE(sceneName) \
    CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(sceneName); \
    CE_DECLARE_SCENE_CREATE_FUNCTION(sceneName); \
    CE_DECLARE_SCENE_RUN_FUNCTION(sceneName);

#endif // CORGO_ENGINE_CORE_SCENE_H
