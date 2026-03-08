//
//  engine/components/scene_script.c
//  Scene script component implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_SCENE_SCRIPT_COMPONENT)
{
    component->m_state = CE_SCENE_STATE_UNLOADED;
    component->m_scriptData = NULL;
    component->m_loadSceneDataFunction = NULL;
    CE_Scene_Clear(&component->m_activeScene);
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_SCENE_SCRIPT_COMPONENT)
{
    // Although the scene may be loaded, since this is a global component we'll let the ECS system destroy the entities and components
    return CE_OK;
}

CE_Result CE_Scene_RequestLoad(INOUT struct CE_ECS_Context* context, IN CE_LoadSceneDataFunction loadFunction, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Debug("Scene Load Request");
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_SCENE_SCRIPT_COMPONENT, sceneScriptComp);
    sceneScriptComp->m_loadSceneDataFunction = loadFunction;

    if (sceneScriptComp->m_state == CE_SCENE_STATE_UNLOADED) {
        CE_Debug("No previous scene loaded.");
        sceneScriptComp->m_state = CE_SCENE_STATE_START_LOAD;
    } else {
        CE_Debug("A scene is already loaded. Unloading current scene before loading new one.");
        sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADING;
    }

    return CE_OK;
}

CE_Result CE_Scene_RequestUnload(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Debug("Scene Unload Request");
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_SCENE_SCRIPT_COMPONENT, sceneScriptComp);

    if (sceneScriptComp->m_state == CE_SCENE_STATE_UNLOADED || sceneScriptComp->m_state == CE_SCENE_STATE_UNLOADING) {
        return CE_OK;
    }

    if (sceneScriptComp->m_state == CE_SCENE_STATE_RUNNING) {
        CE_Debug("Unloading current scene: %s.", sceneScriptComp->m_activeScene.m_id);
        sceneScriptComp->m_state = CE_SCENE_STATE_UNLOADING;
        return CE_OK;
    }
    
    // If we get here it is because the scene is currently loading
    // TOOD: Handle this case
    return CE_ERROR;
}

bool CE_Scene_IsSceneRunning(INOUT struct CE_ECS_Context* context)
{
    return CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_SCRIPT_COMPONENT)->m_state == CE_SCENE_STATE_RUNNING;
}
