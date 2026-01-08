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

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_Cleanup(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (CE_ECS_MainStorage_cleanup(&context->m_storage, context, errorCode) != CE_OK) {
        return CE_ERROR;
    }

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
    // Just call destroy on storage
    return CE_ECS_MainStorage_destroyEntity(&context->m_storage, entity, errorCode);
}