//
//  ecs/core/entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "entity.h"
#include "ecs/ecs.h"
#include "storage.h"
#include "engine/core/platform.h"

CE_Result CE_Entity_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    
    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[componentType];

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }
    
    CE_Id newComponentId = CE_INVALID_ID;
    void *newComponentData = NULL;
    result = CE_ECS_MainStorage_createComponent(&context->m_storage, componentDataPtr, &newComponentId, &newComponentData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // Set output component ID and register component with entity
    result = CE_Bitset_setBit(&entityData->m_entityComponentBitset, componentType);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_BITSET_INDEX_OUT_OF_BOUNDS);
        return CE_ERROR;
    }

    if (cc_insert(&entityData->m_components, newComponentId) == NULL)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    };
    
    *componentId = newComponentId;

    if (componentData) {
        *componentData = newComponentData;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return result;
}

CE_Result CE_Entity_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    const CE_TypeId componentType = CE_Id_getComponentTypeId(componentId);

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    if (cc_get(&entityData->m_components, componentId) == NULL) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    // All good, delete
    const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[componentType];
    result = CE_ECS_MainStorage_destroyComponent(&context->m_storage, componentDataPtr, componentId, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    if (!cc_erase(&entityData->m_components, componentId)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    // Then check the component vector to see if this was the last component of its type
    bool lastComponent = true;
    cc_for_each( &entityData->m_components, el )
    {
        if (CE_Id_getComponentTypeId(*el) == componentType) {
            lastComponent = false;
            break;
        }
    }

    if (lastComponent) {
        CE_Bitset_clearBit(&entityData->m_entityComponentBitset, componentType);
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
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

CE_Result CE_Entity_GetComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    const CE_TypeId componentType = CE_Id_getComponentTypeId(componentId);

    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    if (cc_get(&entityData->m_components, componentId) == NULL) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    if (componentData != NULL) {
        void *dataPtr = NULL;
        const CE_ECS_ComponentStaticData *staticComponentDataPtr = &context->m_componentDefinitions[componentType];
        dataPtr = CE_ECS_ComponentStorage_getComponentDataPointerById(context->m_storage.m_componentTypeStorage[componentType], staticComponentDataPtr, componentId);
        if (dataPtr == NULL) {
            CE_Debug("Component data not found in storage for componentId %u of type %u", componentId, componentType);
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_NOT_FOUND);
            return CE_ERROR;
        }
        *componentData = dataPtr;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_Entity_FindFirstComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_OK;
    
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    const CE_ECS_ComponentStaticData *staticComponentDataPtr = &context->m_componentDefinitions[componentType];

    cc_for_each(&entityData->m_components, el) 
    {
        if (CE_Id_getComponentTypeId(*el) == componentType) {
            void *dataPtr = NULL;
            dataPtr = CE_ECS_ComponentStorage_getComponentDataPointerById(context->m_storage.m_componentTypeStorage[componentType], staticComponentDataPtr, *el);
            if (dataPtr == NULL) {
                CE_Debug("Component data not found in storage for componentId %u of type %u", *el, componentType);
                CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_NOT_FOUND);
                result = CE_ERROR;
            } else {
                *componentId = *el;
                *componentData = dataPtr;
            }
            break;
        }
    }

    if (errorCode != NULL && result == CE_OK) {
        *errorCode = CE_ERROR_CODE_NONE;
    }
    return result;
}

CE_Result CE_Entity_FindAllComponents(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_OK;
    
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    int index = 0;

    cc_for_each(&entityData->m_components, el) 
    {
        if (CE_Id_getComponentTypeId(*el) == componentType) {
            results[index++] = *el;
            if (index >= bufsize) {
                break;
            }
        }
    }

    *resultCount = index;

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return result;
}


CE_Result CE_Entity_GetAllComponentsIter(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id_Set **components, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_OK;
    
    if (entity == CE_INVALID_ID) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, errorCode);
    if (result != CE_OK) {
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }
    
    *components = &entityData->m_components;
    
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return result;
}

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

    CE_Id newRel = CE_INVALID_ID;
    result = CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, relationshipType, 0, targetEntity, &newRel);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    if (cc_insert(&entityData->m_relationships, newRel) == NULL)
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    };

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}
