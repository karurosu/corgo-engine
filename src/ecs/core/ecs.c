//
//  ecs/ecs.c
//  Primary ECS C file, header is ecs/ecs.h
//  This file is in /ecs/core to keep only header files at the root of ecs/
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2025 Carlos Camacho.
//

#include "../ecs.h"

CE_Result CE_ECS_Init(OUT CE_ECS_Context* context, OUT CE_ERROR_CODE *errorCode)
{
    // Clear all component definitions
    for (uint32_t i = 0; i < CE_COMPONENT_TYPE_COUNT; i++) {
        context->m_componentDefinitions[i] = (CE_ECS_ComponentStaticData){ .m_bitmask = 0, .m_isValid = false, .m_type = 0, .m_uid = 0 };
    }

    // Gather component descriptions into the context
#define X(name, uid, storage) name##_description(&context->m_componentDefinitions[name]);
    CE_COMPONENT_DESC_CORE(X)
    #ifndef CE_CORE_TEST_MODE
    CE_COMPONENT_DESC_ENGINE(X)
    CE_COMPONENT_DESC_GAME(X)
    #endif
#undef X

	// Initialize storage structure
	context->m_storage = (CE_ECS_MainStorage){ .m_initialized = false };

    // Initialize main storage
    if (CE_ECS_MainStorage_init(&context->m_storage, context, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_Cleanup(OUT CE_ECS_Context* context, OUT CE_ERROR_CODE* errorCode)
{
    if (CE_ECS_MainStorage_cleanup(&context->m_storage, errorCode) != CE_OK) {
        return CE_ERROR;
    }

	CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

