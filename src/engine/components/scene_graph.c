//
//  engine/components/scene_graph.c
//  Scene graph component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

#include "engine/core/memory.h"
#include "engine/assets.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_SCENE_GRAPH_COMPONENT)
{
    component->m_rootEntityId = CE_INVALID_ID;
    component->m_rebuildZOrderCache = true;
    component->m_needsRedraw = true;

    cc_init(&component->m_zOrderCache);
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_SCENE_GRAPH_COMPONENT)
{
    cc_cleanup(&component->m_zOrderCache);
    return CE_OK;
}

CE_Result CE_Engine_SceneGraph_Init(CE_ECS_Context* context, CE_ERROR_CODE* errorCode)
{
    // Access the scene graph component to ensure it's initialized
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);

    if (sceneGraph->m_rootEntityId == CE_INVALID_ID)
    {
        // Create the root entity
        CE_Result result = CE_ECS_CreateEntity(context, &sceneGraph->m_rootEntityId, errorCode);
        if (result != CE_OK)
        {
            CE_Error("Failed to create root entity for scene graph");
            return CE_ERROR;
        }

        // Add transform component to root entity
        CE_TransformComponent* transformComponent = NULL;
        CE_Id transformComponentId = CE_INVALID_ID;
        result = CE_Entity_AddComponent(context, sceneGraph->m_rootEntityId, CE_TRANSFORM_COMPONENT, &transformComponentId, (void**)&transformComponent, errorCode);
        if (result != CE_OK)
        {
            CE_Error("Failed to add TransformComponent to root entity of scene graph");
            return CE_ERROR;
        }
    }

    return CE_OK;
}
