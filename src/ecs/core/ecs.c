//
//  ecs/ecs.c
//  Primary ECS C file, header is ecs/ecs.h
//  This file is in /ecs/core to keep only header files at the root of ecs/
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2025 Carlos Camacho.
//

#include "../ecs.h"
#include "engine/core/platform.h"

CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE *errorCode)
{
    // Clear all component definitions
    for (uint32_t i = 0; i < CE_COMPONENT_TYPES_COUNT; i++) {
        context->m_componentDefinitions[i] = (CE_ECS_ComponentStaticData){ .m_bitmask = 0, .m_isValid = false, .m_type = 0, .m_uid = 0 };
    }

    CE_Debug("Initializing ECS context");
    // Gather component descriptions into the context
#define X(name, uid, storage, initial_capacity) name##_description(&context->m_componentDefinitions[name]);
    CE_COMPONENT_DESC_CORE(X)
    CE_COMPONENT_DESC_ENGINE(X)
    #ifndef CE_CORE_TEST_MODE
    CE_COMPONENT_DESC_GAME(X)
    #endif
#undef X

#ifdef CE_DEBUG_BUILD
    #define X(name, uid, storage, initial_capacity) CE_Debug("Registered component: %s (Type: %d, UID: %d, Initial Capacity: %zu)", #name, name, name##_UID, name##_InitialCapacity);
        CE_COMPONENT_DESC_CORE(X)
        CE_COMPONENT_DESC_ENGINE(X)
        #ifndef CE_CORE_TEST_MODE
        CE_COMPONENT_DESC_GAME(X)
        #endif
    #undef X
#endif
    
	// Initialize storage structure
	context->m_storage = (CE_ECS_MainStorage){ .m_initialized = false };

    // Initialize main storage
    if (CE_ECS_MainStorage_init(&context->m_storage, context, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    CE_Debug("ECS context initialized successfully");
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_Cleanup(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Debug("Cleaning up ECS context");
    if (CE_ECS_MainStorage_cleanup(&context->m_storage, context, errorCode) != CE_OK) {
        return CE_ERROR;
    }
    CE_Debug("ECS context cleaned up successfully");
	CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_Tick(INOUT CE_ECS_Context* context, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)
{
    // TODO
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

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
    
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, &localErrorCode);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, localErrorCode);
        return CE_ERROR;
    }

    bool success = true;
    // Cleanup components and relationships associated with this entity before destroying it
    for (size_t i = 0; i < cc_size(&entityData->m_components); ++i) {
        const CE_Id componentId = *cc_get(&entityData->m_components, i);
        const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[CE_Id_getComponentTypeId(componentId)];
        if (CE_ECS_MainStorage_destroyComponent(&context->m_storage, componentDataPtr, componentId, &localErrorCode) != CE_OK) {
            // Just print and error and continue, its not ideal but we want to ensure cleanup continues
            CE_Error("Failed to destroy component with ID %d with code %s", componentId, CE_GetErrorMessage(localErrorCode));
            success = false;
        }
    }

    // TODO: cleanup relationships

    if (!success) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
        return CE_ERROR;
    }

    //CE_ECS_MainStorage_destroyEntity clears the entity data, now that memory has been freed
    return CE_ECS_MainStorage_destroyEntity(&context->m_storage, entity, errorCode);
}

CE_Result CE_ECS_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    CE_ERROR_CODE localErrorCode = CE_ERROR_CODE_NONE;
    
    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[componentType];

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, &localErrorCode);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, localErrorCode);
        return CE_ERROR;
    }
    
    CE_Id newComponentId = CE_INVALID_ID;
    void *newComponentData = NULL;
    result = CE_ECS_MainStorage_createComponent(&context->m_storage, componentDataPtr, &newComponentId, &newComponentData, &localErrorCode);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, localErrorCode);
        return CE_ERROR;
    }

    // Set output component ID and register component with entity
    result = CE_Bitset_setBit(&entityData->m_entityComponentBitset, componentType);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_BITSET_INDEX_OUT_OF_BOUNDS);
        return CE_ERROR;
    }

    if (cc_push(&entityData->m_components, newComponentId) == NULL)
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

CE_Result CE_ECS_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result;
    CE_ERROR_CODE localErrorCode = CE_ERROR_CODE_NONE;
    const CE_TypeId componentType = CE_Id_getComponentTypeId(componentId);

    if (componentType == CE_INVALID_TYPE_ID || componentType >= CE_MAX_COMPONENT_TYPES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_TYPE);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = NULL;
    result = CE_ECS_MainStorage_getEntityData(&context->m_storage, entity, &entityData, &localErrorCode);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, localErrorCode);
        return CE_ERROR;
    }

    // First check that the entity actually has this component
    if (!CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, componentType)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    // Then check the component vector
    size_t componentIndex = 0;
    for (componentIndex = 0; componentIndex < cc_size(&entityData->m_components); ++componentIndex) {
        if ( *cc_get(&entityData->m_components, componentIndex) == componentId) {
            break;
        }
    }
    if (componentIndex == cc_size(&entityData->m_components)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY);
        return CE_ERROR;
    }

    // All good, delete
    const CE_ECS_ComponentStaticData *componentDataPtr = &context->m_componentDefinitions[componentType];
    result = CE_ECS_MainStorage_destroyComponent(&context->m_storage, componentDataPtr, componentId, &localErrorCode);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, localErrorCode);
        return CE_ERROR;
    }

    cc_erase(&entityData->m_components, componentIndex);

    bool lastComponent = true;
    for (int i = 0; i < cc_size(&entityData->m_components); ++i) {
        const CE_Id tmpId = *cc_get(&entityData->m_components, i);
        if (CE_Id_getComponentTypeId(tmpId) == componentType) {
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
