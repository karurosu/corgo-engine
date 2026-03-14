//
//  engine/systems/scene_script.c
//  System that deals with scene scripts.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_GLOBAL_SCENE_SCRIPT_SYSTEM)
{
    CE_SceneScriptComponent* sceneScriptComp = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_SCRIPT_COMPONENT);

    switch(sceneScriptComp->m_state)
    {
        case CE_SCENE_STATE_UNLOADED:
            // No active scene, nothing to do
            break;
        case CE_SCENE_STATE_START_LOAD:
            // Requested to load a scene, let's get the scene data
            if (sceneScriptComp->m_loadSceneDataFunction != NULL) {
                if (sceneScriptComp->m_loadSceneDataFunction(context, &sceneScriptComp->m_activeScene, errorCode) != CE_OK) {
                    CE_Error("Failed to load scene data with error: %d", CE_GetErrorMessage(*errorCode));
                    sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
                    break;
                }
                // Check scene data
                if (sceneScriptComp->m_activeScene.m_id == NULL 
                    || sceneScriptComp->m_activeScene.m_createFunction == NULL) {
                    CE_Error("Invalid scene data loaded. Resetting.");
                    sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
                    break;
                }
                // All good, let's load
                CE_Debug("Scene data loaded successfully. Loading scene: %s", sceneScriptComp->m_activeScene.m_id);
                sceneScriptComp->m_state = CE_SCENE_STATE_LOADING;
                sceneScriptComp->m_loadSceneDataFunction = NULL;

                // Initialize scene graph first so other systems can tick
                if (CE_Engine_SceneGraph_Init(context, errorCode) != CE_OK) {
                    CE_Error("Failed to initialize scene graph: %s", CE_GetErrorMessage(*errorCode));
                    return -1;
                }
            } else {
                CE_Error("Scene load requested but no load function set. Resetting.");
                sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
            }
            break;
        case CE_SCENE_STATE_LOADING:
            // TODO: Asset loading and loading screen
            // Create script data component if needed
            if (sceneScriptComp->m_activeScene.m_scriptDataComponentType != CE_INVALID_TYPE_ID) {
                if (CE_Entity_AddComponent(context, CE_Scene_GetRootId(context), sceneScriptComp->m_activeScene.m_scriptDataComponentType, NULL, &sceneScriptComp->m_scriptData, errorCode) != CE_OK) {
                    CE_Error("Failed to create script data component for scene: %s. Error: %s", sceneScriptComp->m_activeScene.m_id, CE_GetErrorMessage(*errorCode));
                    sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
                    CE_Engine_Scene_Clear(&sceneScriptComp->m_activeScene);
                    break;
                }
            }

            // Now run the create function to set up the scene
            if (sceneScriptComp->m_activeScene.m_createFunction != NULL) {
                CE_Debug("Running scene creation script for: %s", sceneScriptComp->m_activeScene.m_id);
                if (sceneScriptComp->m_activeScene.m_createFunction(context, sceneScriptComp->m_scriptData, errorCode) != CE_OK) {
                    CE_Error("Failed to create scene with error: %d", CE_GetErrorMessage(*errorCode));
                    sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADING; // Go to unloading to clean up any partial entities/components created during the failed create
                    CE_Engine_Scene_Clear(&sceneScriptComp->m_activeScene);
                    break;
                }
                CE_Debug("Scene created successfully. Running scene: %s", sceneScriptComp->m_activeScene.m_id);
                sceneScriptComp->m_state = CE_SCENE_STATE_RUNNING;
            } else {
                CE_Error("No create function set when loading the scene. Resetting.");
                sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
                CE_Engine_Scene_Clear(&sceneScriptComp->m_activeScene);
                break;
            }

            break;
        case CE_SCENE_STATE_RUNNING:
            // Run the scene if there is a function defined
            if (sceneScriptComp->m_activeScene.m_runFunction != NULL) {
                if (sceneScriptComp->m_activeScene.m_runFunction(context, deltaTime, errorCode) != CE_OK) {
                    // Run errors are logged but not fatal
                    CE_Error("Failed to run scene: %s. Error: %s", sceneScriptComp->m_activeScene.m_id, CE_GetErrorMessage(*errorCode));
                }
            }
            break;
        case CE_SCENE_STATE_UNLOADING:
            CE_Debug("Unloading scene: %s", sceneScriptComp->m_activeScene.m_id);
            // Clear scene graph
            if (CE_Engine_SceneGraph_Reset(context, errorCode) == CE_ERROR) {
                CE_Error("Failed to reset scene graph when unloading scene: %s. Error: %s", sceneScriptComp->m_activeScene.m_id, CE_GetErrorMessage(*errorCode));
            }

            // Clear currrent scene
            CE_Engine_Scene_Clear(&sceneScriptComp->m_activeScene);

            // Load next scene if one is pending
            if (sceneScriptComp->m_loadSceneDataFunction != NULL) {
                CE_Debug("Scene unload requested by switching scenes.");
                sceneScriptComp->m_state = CE_SCENE_STATE_START_LOAD;
            } else {
                CE_Debug("No new scene load requested. Scene is now unloaded.");
                sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADED;
            }
            break;
        default:
            CE_Error("Invalid scene state");
            return CE_ERROR;
    }

    return CE_OK;
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION
