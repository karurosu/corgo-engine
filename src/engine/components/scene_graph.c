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

CE_Result CE_SceneGraph_AddChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, IN bool move, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    
    sceneGraph->m_rebuildZOrderCache = true;
    sceneGraph->m_needsRedraw = true;

    if (CE_Entity_HasRelationship(context, childId, CE_RELATIONSHIP_PARENT)) {
        if (!move) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_ALREADY_PARENTED);
            return CE_ERROR;
        }

        if (CE_Entity_RemoveRelationship(context, childId, CE_RELATIONSHIP_PARENT, parentId, errorCode) != CE_OK) {
            return CE_ERROR;
        }
    }

    return CE_Entity_AddRelationship(context, parentId, CE_RELATIONSHIP_CHILD, childId, errorCode);
}

CE_Result CE_SceneGraph_RemoveChild(INOUT CE_ECS_Context* context, IN CE_Id parentId, IN CE_Id childId, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    
    sceneGraph->m_rebuildZOrderCache = true;
    sceneGraph->m_needsRedraw = true;

    bool exists = false;
    if (CE_Entity_HasSpecificRelationship(context, childId, CE_RELATIONSHIP_PARENT, parentId, &exists, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (!exists) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_ENTITY_DOES_NOT_HAVE_CHILD);
        return CE_ERROR;
    }

    return CE_Entity_RemoveRelationship(context, parentId, CE_RELATIONSHIP_CHILD, childId, errorCode);
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
    if (!cc_reserve(&expansionList, 6)) // Reserve an average of 3 children per entity
    {
        CE_Error("Failed to reserve memory for CacheZOrderTraverseFunc expansion list");
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
                NodeInfo childNode = { .entityId = 0, .parentId = currentNode->entityId };
                CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, CE_Id_getGeneration(*childRel), CE_Id_getUniqueId(*childRel), &childNode.entityId);
                cc_push(&expansionList, childNode);
            }
        }
    }

    cc_cleanup(&expansionList);
    return CE_OK;
}

CE_Result CacheZOrderTraverseFunc(IN CE_ECS_Context* context, IN CE_Id entityId, IN CE_Id parentId, INOUT void* userData, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    CE_TransformComponent* transformComponent = NULL;
    CE_Id componentId = CE_INVALID_ID;

    if (CE_Entity_FindFirstComponent(context, entityId, CE_TRANSFORM_COMPONENT, &componentId, (void**)&transformComponent, errorCode) != CE_OK) {
        CE_Error("Failed to find transform component for entity %u while rebuilding scene graph Z-order cache", entityId);
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_SCENE_GRAPH_MISSING_TRANSFORM);
        return CE_ERROR;
    }

    CESceneGraphRenderNode renderNode = {
        .m_entityShortId = CE_Id_getUniqueId(entityId),
        .m_zIndex = transformComponent->z
    };

    cc_insert(&sceneGraph->m_zOrderCache, renderNode);

    return CE_OK;
}

CE_Result CE_Engine_SceneGraph_RebuildZOrderCache(INOUT CE_ECS_Context* context, CE_ERROR_CODE* errorCode)
{
    CE_SceneGraphComponent* sceneGraph = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_SCENE_GRAPH_COMPONENT);
    
    if (!sceneGraph->m_rebuildZOrderCache)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
        return CE_OK;
    }

    // Remove old cache
    cc_clear(&sceneGraph->m_zOrderCache);

    if (sceneGraph->m_rootEntityId == CE_INVALID_ID)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_SCENE_GRAPH_NOT_READY);
        return CE_ERROR;
    }
    
    // Traverse the scene graph and rebuild the cache
    if (CE_Engine_SceneGraph_Traverse(context, sceneGraph->m_rootEntityId, CacheZOrderTraverseFunc, NULL, errorCode) != CE_OK)
    {
        return CE_ERROR;
    }

    sceneGraph->m_rebuildZOrderCache = false;
    return CE_OK;
}
