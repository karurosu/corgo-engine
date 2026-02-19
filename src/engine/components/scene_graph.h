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
    int16_t m_width;
    int16_t m_height;
} CE_SceneGraphRenderNode;

#define CC_CMPR CE_SceneGraphRenderNode, { return val_1.m_z < val_2.m_z ? -1 : val_1.m_z > val_2.m_z; }
#include <cc.h>

typedef struct CE_SceneGraphComponent {
    CE_Id m_rootEntityId;
    bool m_needsRedraw; // Indicates that the scene needs to be redrawn, used to avoid unnecessary redraws
    cc_omap(uint16_t, CE_SceneGraphRenderNode) m_renderList; // Cache entity coordinates for rendering, sorted by Z-order
} CE_SceneGraphComponent;

typedef CE_Result (*CE_SceneGraphTraverseCallback)(IN CE_ECS_Context* context, IN CE_Id entityId, IN CE_Id parentId, INOUT void* userData, CE_ERROR_CODE* errorCode);

/// Private API
CE_Result CE_Engine_SceneGraph_Init(CE_ECS_Context* context, CE_ERROR_CODE* errorCode);
CE_Result CE_Engine_SceneGraph_DeleteRenderNode(IN CE_ECS_Context* context, IN CE_Id entityId);
CE_Result CE_Engine_SceneGraph_Traverse(INOUT CE_ECS_Context* context, IN CE_Id entityId, IN CE_SceneGraphTraverseCallback callback, INOUT void* userData, CE_ERROR_CODE* errorCode);
CE_Result CE_Engine_SceneGraph_UpdateRenderList(INOUT CE_ECS_Context* context, CE_ERROR_CODE* errorCode);

#define CE_Engine_SceneGraph_MarkDirty(contextPtr) do {CE_ECS_AccessGlobalComponent(contextPtr, CE_ENGINE_SCENE_GRAPH_COMPONENT)->m_needsRedraw = true;} while(0)

/// Public API

/** 
 * @brief Mecro: Get the root entity ID of the scene graph.
 * 
 * @param contextPtr[in] The ECS context.
 * @return The root entity ID.
 */
#define CE_SceneGraph_GetSceneRootId(contextPtr) \
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

/**
 * @brief Gets the render node for a given entity ID from the scene graph cache.
 * Returns NULL if the entity does not have a render node in the cache.
 * 
 * @param context[in] The ECS context.
 * @param entityId[in] The entity ID to get the render node for.
 * @return The render node for the entity, or NULL if it does not exist in the cache.
 */
CE_SceneGraphRenderNode *CE_SceneGraph_GetRenderNode(IN CE_ECS_Context* context, IN CE_Id entityId);

#endif // CORGO_ENGINE_COMPONENTS_SCENE_GRAPH_H
