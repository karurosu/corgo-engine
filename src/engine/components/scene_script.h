//
//  engine/components/scene_script.h
//  Scene script component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_SCENE_SCRIPT_H
#define CORGO_ENGINE_COMPONENTS_SCENE_SCRIPT_H

#include "ecs/types.h"
#include "engine/core/scene.h"

typedef struct CE_SceneScriptComponent
{
    CE_SceneState m_state;
    CE_LoadSceneDataFunction m_loadSceneDataFunction;
    void *m_scriptData;
    CE_Scene m_activeScene;
} CE_SceneScriptComponent;

#endif // CORGO_ENGINE_COMPONENTS_SCENE_SCRIPT_H
