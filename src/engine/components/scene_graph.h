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
    int16_t m_x;
    int16_t m_y;
    int16_t m_z;
} CESceneGraphRenderNode;

#define CC_CMPR CESceneGraphRenderNode, { return val_1.m_z < val_2.m_z ? -1 : val_1.m_z > val_2.m_z; }
#include <cc.h>

typedef struct CESceneGraphComponent {
    CE_Id m_rootEntityId;
    bool m_rebuildZOrderCache;
    bool m_needsRedraw;
    cc_omap(uint16_t, CESceneGraphRenderNode) m_renderList; // Cache entity coordinates for rendering, sorted by Z-order
} CE_SceneGraphComponent;

// Helpers
CE_Result CE_Engine_SceneGraph_Init(CE_ECS_Context* context, CE_ERROR_CODE* errorCode);

#define CE_SceneGraph_GetSceneRootId(contextPtr) \
    (CE_ECS_AccessGlobalComponent((contextPtr), CE_ENGINE_SCENE_GRAPH_COMPONENT)->m_rootEntityId)

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

/// Private API

typedef CE_Result (*CE_SceneGraphTraverseCallback)(IN CE_ECS_Context* context, IN CE_Id entityId, IN CE_Id parentId, INOUT void* userData, CE_ERROR_CODE* errorCode);

CE_Result CE_Engine_SceneGraph_Traverse(INOUT CE_ECS_Context* context, IN CE_Id entityId, IN CE_SceneGraphTraverseCallback callback, INOUT void* userData, CE_ERROR_CODE* errorCode);
CE_Result CE_Engine_SceneGraph_RebuildRenderList(INOUT CE_ECS_Context* context, CE_ERROR_CODE* errorCode);

/// Public API

/** 
 * @brief Adds a child entity to a parent entity in the scene graph.
 * Ensures caches are updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param parentId[in] The parent entity ID.
 * @param childId[in] The child entity ID.
 * @param move[in] Whether to move the child from its current parent if it already has one.
 * @param errorCode[out] The error code in case of failure.
 */
CE_Result CE_SceneGraph_AddChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, IN bool move, CE_ERROR_CODE* errorCode);

/**
 * @brief Removes a child entity from a parent entity in the scene graph.
 * Ensures caches are updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param parentId[in] The parent entity ID.
 * @param childId[in] The child entity ID.
 * @param errorCode[out] The error code in case of failure.
 */
CE_Result CE_SceneGraph_RemoveChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, CE_ERROR_CODE* errorCode);

#endif // CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H