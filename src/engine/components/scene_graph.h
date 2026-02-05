//
//  engine/components/scene_graph.h
//  Scene graph component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H
#define CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H

#include "ecs/types.h"
#include "ecs/core/ecs_component.h"

typedef struct {
    uint16_t m_entityShortId;
    uint16_t m_zIndex;
} CESceneGraphRenderNode;

#define CC_CMPR CESceneGraphRenderNode, { return val_1.m_zIndex < val_2.m_zIndex ? -1 : val_1.m_zIndex > val_2.m_zIndex; }
#define CC_HASH CESceneGraphRenderNode, { return val.m_entityShortId * 2654435761ull; }
#include <cc.h>

typedef struct CESceneGraphComponent {
    CE_Id m_rootEntityId;
    bool m_rebuildZOrderCache;
    bool m_needsRedraw;
    cc_oset(CESceneGraphRenderNode) m_zOrderCache;
} CE_SceneGraphComponent;

// Helpers
CE_Result CE_Engine_SceneGraph_Init(CE_ECS_Context* context, CE_ERROR_CODE* errorCode);

#define CE_Engine_GetSceneRootId(contextPtr) \
    ((CE_ECS_AccessGlobalComponent((contextPtr), CE_ENGINE_SCENE_GRAPH_COMPONENT))->m_rootEntityId)

#define CE_Engine_SceneGraph_MarkDirty(contextPtr) \
    do { \
        CE_SceneGraphComponent* sceneGraphComp = CE_ECS_AccessGlobalComponent(contextPtr, CE_ENGINE_SCENE_GRAPH_COMPONENT); \
        sceneGraphComp->m_needsRedraw = true; \
    } while(0)

#define CE_Engine_SceneGraph_MarkZOrderDirty(contextPtr) \
    do { \
        CE_SceneGraphComponent* sceneGraphComp = CE_ECS_AccessGlobalComponent(contextPtr, CE_ENGINE_SCENE_GRAPH_COMPONENT); \
        sceneGraphComp->m_rebuildZOrderCache = true; \
    } while(0)

#endif // CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H