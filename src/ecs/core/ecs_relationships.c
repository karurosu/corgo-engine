//
//  ecs/core/ecs_relationships.c
//  Live Relationships manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "ecs_relationships.h"

#include "id.h"
#include "storage.h"
#include "engine/core/platform.h"

CE_Result CE_Entity_AddRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, IN CE_TypeId reciprocalRelationshipType, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;

    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    CE_ECS_EntityData* otherEntityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, targetEntity, &otherEntityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    CE_Id newRel = CE_INVALID_ID;
    result = CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, relationshipType, CE_Id_getGeneration(targetEntity), CE_Id_getUniqueId(targetEntity), &newRel);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (cc_insert(&entityData->m_relationships, newRel) == NULL)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    };

    CE_Bitset_setBit(&entityData->m_entityRelationshipBitset, relationshipType);

    if (reciprocalRelationshipType != CE_INVALID_TYPE_ID) {
        result = CE_Entity_AddRelationship(context, targetEntity, reciprocalRelationshipType, entity, CE_INVALID_TYPE_ID, errorCode);
        if (result != CE_OK) {
            return CE_ERROR;
        }
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_RemoveRelationship(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId relationshipType, IN CE_Id targetEntity, IN CE_TypeId reciprocalRelationshipType, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;

    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (relationshipType == CE_INVALID_TYPE_ID || relationshipType >= CE_MAX_RELATIONSHIP_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_RELATIONSHIP_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    if (!CE_Bitset_isBitSet(&entityData->m_entityRelationshipBitset, relationshipType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP);
        return CE_ERROR;
    }

    CE_Id newRel = CE_INVALID_ID;
    result = CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, relationshipType, CE_Id_getGeneration(targetEntity), CE_Id_getUniqueId(targetEntity), &newRel);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (!cc_erase(&entityData->m_relationships, newRel))
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

    if (reciprocalRelationshipType != CE_INVALID_TYPE_ID) {
        result = CE_Entity_RemoveRelationship(context, targetEntity, reciprocalRelationshipType, entity, CE_INVALID_TYPE_ID, errorCode);
        if (result != CE_OK) {
            return CE_ERROR;
        }
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}
