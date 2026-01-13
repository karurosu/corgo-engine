//
//  ecs/core/entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "entity.h"
#include "ecs/ecs.h"
#include "storage.h"

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
