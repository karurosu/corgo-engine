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
    component->m_needsRedraw = true;

    cc_init(&component->m_renderList);
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_SCENE_GRAPH_COMPONENT)
{
    cc_cleanup(&component->m_renderList);
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
        transformComponent->m_inSceneGraph = true;
    }

    return CE_OK;
}

CE_Result CE_SceneGraph_AddChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, IN bool move, CE_ERROR_CODE* errorCode)
{
    // Check that the child has a transform component
    CE_TransformComponent* transformComponent = NULL;
    if (!CE_Entity_HasComponent(context, childId, CE_TRANSFORM_COMPONENT))
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_SCENE_GRAPH_MISSING_TRANSFORM);
        return CE_ERROR;
    }
    
    // Add the relationship
    if (CE_Entity_HasRelationship(context, childId, CE_RELATIONSHIP_PARENT)) {
        if (!move) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_ALREADY_PARENTED);
            return CE_ERROR;
        }

        // If the child already has a parent, remove that relationship first
        if (CE_Entity_RemoveRelationship(context, childId, CE_RELATIONSHIP_PARENT, parentId, errorCode) != CE_OK) {
            return CE_ERROR;
        }
    }

    // Parent the child
    if (CE_Entity_AddRelationship(context, parentId, CE_RELATIONSHIP_CHILD, childId, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // Mark the transform as being in the scene graph
    CE_TransformComponent *transformComp = NULL;
    CE_Id componentId = CE_INVALID_ID;
    if (CE_Entity_FindFirstComponent(context, childId, CE_TRANSFORM_COMPONENT, &componentId, (void**)&transformComp, errorCode) != CE_OK) {
        return CE_ERROR;
    }
    transformComp->m_inSceneGraph = true;

    // Adding a new component needs a redraw
    CE_Engine_SceneGraph_MarkDirty(context);

    return CE_OK;
}

CE_Result CE_SceneGraph_RemoveChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);

    bool exists = false;
    if (CE_Entity_HasSpecificRelationship(context, childId, CE_RELATIONSHIP_PARENT, parentId, &exists, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (!exists) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_ENTITY_DOES_NOT_HAVE_CHILD);
        return CE_ERROR;
    }

    if (CE_Entity_RemoveRelationship(context, parentId, CE_RELATIONSHIP_CHILD, childId, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // Mark the transform as no longer being in the scene graph
    CE_TransformComponent *transformComp = NULL;
    CE_Id componentId = CE_INVALID_ID;
    if (CE_Entity_FindFirstComponent(context, childId, CE_TRANSFORM_COMPONENT, &componentId, (void**)&transformComp, errorCode) != CE_OK) {
        return CE_ERROR;
    }
    transformComp->m_inSceneGraph = false;

    // Removing a component needs a redraw
    CE_Engine_SceneGraph_MarkDirty(context);

    return CE_OK;
}

// Simple struct used for iterating the scene graph
typedef struct NodeInfo {
    CE_Id entityId;
    CE_Id parentId;
} NodeInfo;

CE_Result CE_Engine_SceneGraph_Traverse(INOUT CE_ECS_Context* context, IN CE_Id entityId, IN CE_SceneGraphTraverseCallback callback, INOUT void* userData, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);

    cc_vec(NodeInfo) expansionList;
    cc_init(&expansionList);
    if (!cc_reserve(&expansionList, 6)) // Reserve an average of 6 children per entity
    {
        CE_Error("Failed to reserve memory for SceneGraph expansion list");
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    }

    NodeInfo rootNode = { .entityId = sceneGraph->m_rootEntityId, .parentId = CE_INVALID_ID };
    cc_push(&expansionList, rootNode);

    while(cc_size(&expansionList) > 0)
    {
        NodeInfo *currentNode = cc_last(&expansionList);
        cc_erase(&expansionList, cc_size(&expansionList) - 1);

        // Process the current entity
        if (callback(context, currentNode->entityId, currentNode->parentId, userData, errorCode) != CE_OK)
        {
            cc_cleanup(&expansionList);
            return CE_ERROR;
        }

        // Get children of the current entity and add them to the expansion list
        CE_Id_Set *children;
        if (CE_Entity_GetAllRelationshipsIter(context, currentNode->entityId, &children, errorCode) != CE_OK)
        {
            cc_cleanup(&expansionList);
            return CE_ERROR;
        }

        cc_for_each(children, childRel)
        {
            if (CE_Id_getRelationshipTypeId(*childRel) == CE_RELATIONSHIP_CHILD) {
                NodeInfo childNode = { .entityId = CE_Id_relationshipToEntityReference(*childRel), .parentId = currentNode->entityId };
                cc_push(&expansionList, childNode);
            }
        }
    }

    cc_cleanup(&expansionList);
    return CE_OK;
}

CE_Result UpdateRenderListTraverseFunc(IN CE_ECS_Context* context, IN CE_Id entityId, IN CE_Id parentId, INOUT void* userData, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    CE_CameraComponent *cameraComponent = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_CAMERA_COMPONENT);
    CE_TransformComponent* transformComponent = NULL;
    CE_Id componentId = CE_INVALID_ID;

    if (CE_Entity_FindFirstComponent(context, entityId, CE_TRANSFORM_COMPONENT, &componentId, (void**)&transformComponent, errorCode) != CE_OK) {
        CE_Error("Failed to find transform component for entity %u while rebuilding scene graph Z-order cache", entityId);
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_SCENE_GRAPH_MISSING_TRANSFORM);
        return CE_ERROR;
    }

    CE_SceneGraphRenderNode *parentRenderNode = cc_get(&sceneGraph->m_renderList, CE_Id_getUniqueId(parentId));

    if (parentRenderNode == NULL && parentId != CE_INVALID_ID) {
        CE_Error("Parent entity %u does not have a render node in the scene graph cache while rebuilding Z-order cache for entity %u", parentId, entityId);
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    const int16_t parentX = parentId == CE_INVALID_ID ? -cameraComponent->m_x : parentRenderNode->m_x;
    const int16_t parentY = parentId == CE_INVALID_ID ? -cameraComponent->m_y : parentRenderNode->m_y;

    CE_SceneGraphRenderNode *renderNode = cc_get(&sceneGraph->m_renderList, CE_Id_getUniqueId(entityId));

    if (renderNode == NULL) {
        CE_SceneGraphRenderNode newNode = {
            .m_x = parentX + transformComponent->m_x,
            .m_y = parentY + transformComponent->m_y,
            .m_z = transformComponent->m_z,
            .m_width = transformComponent->m_width,
            .m_height = transformComponent->m_height
        };
        renderNode = cc_insert(&sceneGraph->m_renderList, CE_Id_getUniqueId(entityId), newNode);
        if (renderNode == NULL) {
            CE_Error("Failed to insert render node for entity %u into scene graph Z-order cache", entityId);
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
            return CE_ERROR;
        }
    }
    else{
        renderNode->m_x = parentX + transformComponent->m_x;
        renderNode->m_y = parentY + transformComponent->m_y;
        // Note that Z is not updated since any changes will delete the render node.
        renderNode->m_width = transformComponent->m_width;
        renderNode->m_height = transformComponent->m_height;
    }
    
    return CE_OK;
}

CE_Result CE_Engine_SceneGraph_DeleteRenderNode(IN CE_ECS_Context* context, IN CE_Id entityId)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    cc_erase(&sceneGraph->m_renderList, CE_Id_getUniqueId(entityId)); // Cannot fail, we don't care if its been deleted already since it will be readded on next rebuild
    sceneGraph->m_needsRedraw = true; // Mark dirty to ensure the render node is removed from the cache on the next redraw
    return CE_OK;
}

CE_Result CE_Engine_SceneGraph_UpdateRenderList(INOUT CE_ECS_Context* context, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);

    if (sceneGraph->m_rootEntityId == CE_INVALID_ID)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_SCENE_GRAPH_NOT_READY);
        return CE_ERROR;
    }

    // Traverse the scene graph and update the cache
    if (CE_Engine_SceneGraph_Traverse(context, sceneGraph->m_rootEntityId, UpdateRenderListTraverseFunc, NULL, errorCode) != CE_OK)
    {
        return CE_ERROR;
    }

    return CE_OK;
}

CE_SceneGraphRenderNode *CE_SceneGraph_GetRenderNode(IN CE_ECS_Context* context, IN CE_Id entityId)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    return cc_get(&sceneGraph->m_renderList, CE_Id_getUniqueId(entityId));
}
