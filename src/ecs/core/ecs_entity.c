//
//  ecs/core/entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "ecs_entity.h"

#include "id.h"
#include "storage.h"
#include "context.h"
#include "engine/core/platform.h"

#include "ecs_relationships.h"

CE_Result CE_ECS_CreateEntity(INOUT CE_ECS_Context* context, OUT CE_Id* outId, OUT_OPT CE_ERROR_CODE* errorCode)
{
    // Just call create on storage
    return CE_ECS_MainStorage_createEntity(&context->m_storage, outId, errorCode);
}

CE_Result CE_ECS_DestroyEntity(INOUT CE_ECS_Context* context, IN CE_Id entity, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    CE_ERROR_CODE localErrorCode = CE_ERROR_CODE_NONE;
    CE_ECS_EntityData* entityData = NULL;
    
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    bool success = true;
    // Cleanup components and relationships associated with this entity before destroying it
    // Do this here since the storage functions do not automatically cleanup associated components and relationships
    cc_for_each(&entityData->m_components, componentIdPtr) 
    {
        const CE_Id componentId = *componentIdPtr;
        const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[CE_Id_getComponentTypeId(componentId)];
        if (CE_ECS_MainStorage_destroyComponent(&context->m_storage, context, componentDataPtr, componentId, &localErrorCode) != CE_OK) {
            // Just print and error and continue, its not ideal but we want to ensure cleanup continues
            CE_Error("Failed to destroy component with ID %d with code %s", componentId, CE_GetErrorMessage(localErrorCode));
            success = false;
        }
    }

    cc_for_each(&entityData->m_relationships, relationshipIdPtr) 
    {
        const CE_Id relationshipId = *relationshipIdPtr;
        CE_Id targetId = CE_INVALID_ID;
        result = CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, CE_Id_getRelationshipTypeId(relationshipId), CE_Id_getGeneration(relationshipId), CE_Id_getUniqueId(relationshipId), &targetId);
        if (result != CE_OK) {
            CE_Error("Failed to make reference ID for relationship ID %d", relationshipId);
            success = false;
            continue;
        }

        if (CE_Entity_RemoveRelationship(context, entity, CE_Id_getRelationshipTypeId(relationshipId), targetId, &localErrorCode) != CE_OK) {
            // Just print and error and continue, its not ideal but we want to ensure cleanup continues
            CE_Error("Failed to remove relationship with ID %d with code %s", relationshipId, CE_GetErrorMessage(localErrorCode));
            success = false;
        }
        
    }

    if (!success) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
        return CE_ERROR;
    }

    //CE_ECS_MainStorage_destroyEntity clears the entity data, now that memory has been freed
    return CE_ECS_MainStorage_destroyEntity(&context->m_storage, entity, errorCode);
}

bool CE_Entity_IsValid(INOUT CE_ECS_Context* context, IN CE_Id entity)
{
    if (entity == CE_INVALID_ID) {
        return false;
    }

    CE_Result result;
    CE_ECS_EntityData* entityData;

    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, NULL);
    return result == CE_OK;
}

size_t CE_Entity_GetComponentCount(INOUT CE_ECS_Context* context, IN CE_Id entity)
{
    CE_Result result;
    CE_ECS_EntityData* entityData;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, NULL);
    if (entityData == NULL || result != CE_OK) {
        return 0;
    }
    return cc_size(&entityData->m_components);
}

size_t CE_Entity_GetRelationshipCount(INOUT CE_ECS_Context* context, IN CE_Id entity)
{
    CE_Result result;
    CE_ECS_EntityData* entityData;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, NULL);
    if (entityData == NULL || result != CE_OK) {
        return 0;
    }
    return cc_size(&entityData->m_relationships);
}

bool CE_Entity_HasComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType)
{
    CE_Result result;
    CE_ECS_EntityData* entityData;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, NULL);
    if (entityData == NULL || result != CE_OK) {
        return 0;
    }
    return CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType);
}

bool CE_Entity_HasRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType)
{
    CE_Result result;
    CE_ECS_EntityData* entityData;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, NULL);
    if (entityData == NULL || result != CE_OK) {
        return 0;
    }
    return CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType);
}

