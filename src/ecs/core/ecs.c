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
        if (CE_ECS_MainStorage_destroyComponent(&context->m_storage, componentDataPtr, componentId, &localErrorCode) != CE_OK) {
            // Just print and error and continue, its not ideal but we want to ensure cleanup continues
            CE_Error("Failed to destroy component with ID %d with code %s", componentId, CE_GetErrorMessage(localErrorCode));
            success = false;
        }
    }

    cc_for_each(&entityData->m_relationships, relationshipIdPtr) 
    {
        // TODO: Destroy relationships associated with this entity
        
    }

    if (!success) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
        return CE_ERROR;
    }

    //CE_ECS_MainStorage_destroyEntity clears the entity data, now that memory has been freed
    return CE_ECS_MainStorage_destroyEntity(&context->m_storage, entity, errorCode);
}

