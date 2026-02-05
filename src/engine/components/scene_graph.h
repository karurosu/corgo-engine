//
//  engine/components/scene_graph.h
//  Scene graph component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H
#define CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H

#include "ecs/types.h"

typedef struct CESceneGraphComponent {
    CE_Id m_rootEntityId;
} CE_SceneGraphComponent;

// Helpers
CE_Result CE_Engine_SceneGraph_Init(CE_ECS_Context* context, CE_ERROR_CODE* errorCode);

#endif // CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H