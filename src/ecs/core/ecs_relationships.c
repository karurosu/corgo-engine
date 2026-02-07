//
//  ecs/core/ecs_relationships.c
//  Live Relationships manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "ecs_relationships.h"

#include "id.h"
#include "storage.h"
#include "context.h"
#include "engine/core/platform.h"
#include "ecs/relationships.h"

CE_Result CE_Entity_AddRelationship_Internal(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id relationshipToAdd, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_EntityData* entityData = NULL;
    if (CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (cc_insert(&entityData->m_relationships, relationshipToAdd) == NULL)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    };

    if (CE_Bitset_setBit(&entityData->m_entityRelationshipBitset, CE_Id_getRelationshipTypeId(relationshipToAdd)) != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_AddRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_Id newRel = CE_INVALID_ID;
    if (CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, relationshipType, CE_Id_getGeneration(targetEntity), CE_Id_getUniqueId(targetEntity), &newRel) != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (CE_Entity_AddRelationship_Internal(context, entity, newRel, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // Make reciprocal relationship
    if (CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, CE_RELATIONSHIPS_RECIPROCALS[relationshipType], CE_Id_getGeneration(entity), CE_Id_getUniqueId(entity), &newRel) != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (CE_Entity_AddRelationship_Internal(context, targetEntity, newRel, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_RemoveRelationship_Internal(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id relationshipToRemove, OUT_OPT CE_ERROR_CODE* errorCode)
{
    const CE_TypeId relationshipType = CE_Id_getRelationshipTypeId(relationshipToRemove);
    CE_ECS_EntityData* entityData = NULL;
    if (CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (!CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP);
        return CE_ERROR;
    }

    if (!cc_erase(&entityData->m_relationships, relationshipToRemove))
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP);
        return CE_ERROR;
    };

    // If no more relationships of this type exist, clear the bit
    bool hasMoreOfType = false;
    cc_for_each(&entityData->m_relationships, el)
    {
        if (CE_Id_getRelationshipTypeId(*el) == relationshipType) {
            hasMoreOfType = true;
            break;
        }
    }

    if (!hasMoreOfType) {
        CE_Bitset_clearBit(&entityData->m_entityRelationshipBitset, relationshipType);
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_RemoveRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_Id newRel = CE_INVALID_ID;
    if (CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, relationshipType, CE_Id_getGeneration(targetEntity), CE_Id_getUniqueId(targetEntity), &newRel) != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (CE_Entity_RemoveRelationship_Internal(context, entity, newRel, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, CE_RELATIONSHIPS_RECIPROCALS[relationshipType], CE_Id_getGeneration(entity), CE_Id_getUniqueId(entity), &newRel) != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (CE_Entity_RemoveRelationship_Internal(context, targetEntity, newRel, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_FindFirstRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, OUT CE_Id* relationshipId, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    if (CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this relationship
    if (!CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP);
        return CE_ERROR;
    }

    cc_for_each(&entityData->m_relationships, el) 
    {
        if (CE_Id_getRelationshipTypeId(*el) == relationshipType) {
            *relationshipId = *el;
            break;
        }
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_FindAllRelationships(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    if (CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this relationship
    if (!CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP);
        return CE_ERROR;
    }

    int index = 0;

    cc_for_each(&entityData->m_relationships, el) 
    {
        if (CE_Id_getRelationshipTypeId(*el) == relationshipType) {
            results[index++] = *el;
            if (index >= bufsize) {
                break;
            }
        }
    }

    *resultCount = index;

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_GetRelationshipForSystem(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, const IN CE_ECS_SystemStaticData *system, OUT CE_Id* targetId, OUT_OPT CE_ERROR_CODE* errorCode)
{
    // TODO: add support for multiple relationships of the same type
    return CE_Entity_FindFirstRelationship(context, entity, relationshipType, targetId, errorCode);
}

CE_Result CE_Entity_HasSpecificRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, OUT bool* exists, OUT_OPT CE_ERROR_CODE* errorCode)
{
    *exists = false;
    
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    if (CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    if (!CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType)) {
        return CE_OK;
    }

    cc_for_each(&entityData->m_relationships, el) 
    {
        if (CE_Id_getRelationshipTypeId(*el) == relationshipType) {
            *exists = (CE_Id_getUniqueId(*el) == CE_Id_getUniqueId(targetEntity) && CE_Id_getGeneration(*el) == CE_Id_getGeneration(targetEntity));
            break;
        }
    }

    return CE_OK;
}
