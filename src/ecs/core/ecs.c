//
//  ecs/ecs.c
//  Primary ECS C file, header is ecs/ecs.h
//  This file is in /ecs/core to keep only header files at the root of ecs/
//  Only publlic functions are implemented here.
//  Copyright (c) 2025 Carlos Camacho.
//

#include "../ecs.h"
#include "ecs_internal.h"
#include "engine/core/platform.h"

CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE *errorCode)
{
    // temp variable to count how many systems are registered per filter
    uint8_t systemCount[CE_ECS_SYSTEM_RUN_ORDER_COUNT][CE_ECS_SYSTEM_RUN_FREQUENCY_COUNT][CE_ECS_SYSTEM_RUN_PHASE_COUNT] = {0};
    
    // Clear all component definitions
    for (uint32_t i = 0; i < CE_COMPONENT_TYPES_COUNT; i++) {
        context->m_componentDefinitions[i] = (CE_ECS_ComponentStaticData){ 
            .m_isValid = false, 
            .m_type = 0, 
            .m_uid = 0,
            .m_storageSizeOf = 0, 
            .m_initialCapacity = 0,
            .m_initFunction = NULL,
            .m_cleanupFunction = NULL
        };
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
    #define X(name, uid, storage, initial_capacity) CE_Debug("Registered component: %s (Type: %d, UID: %d, Initial Capacity: %u)", #name, name, name##_UID, name##_InitialCapacity);
        CE_COMPONENT_DESC_CORE(X)
        CE_COMPONENT_DESC_ENGINE(X)
        #ifndef CE_CORE_TEST_MODE
        CE_COMPONENT_DESC_GAME(X)
        #endif
    #undef X
#endif
    
    // Load system descriptions into context
    for (uint32_t i = 0; i < CE_SYSTEM_TYPES_COUNT; i++) {
        context->m_systemDefinitions[i] = (CE_ECS_SystemStaticData){ 
            .m_enabled = false,
            .m_isValid = false,
            .m_systemId = 0, 
            .m_runOrder = CE_ECS_SYSTEM_RUN_ORDER_AUTO, 
            .m_runPhase = CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, 
            .m_runFrequency = CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY,
            .m_runFunction = NULL
        };
    }

#define X(name, run_order, run_phase, run_frequency, ...) \
    name##_description(&context->m_systemDefinitions[name]);\
    systemCount[run_order][run_frequency][run_phase]++;
    CE_SYSTEM_DESC_CORE(X)
    CE_SYSTEM_DESC_ENGINE(X)
    #ifndef CE_CORE_TEST_MODE
    CE_SYSTEM_DESC_GAME(X)
    #endif
#undef X

#ifdef CE_DEBUG_BUILD
    #define X(name, run_order, run_phase, run_frequency, ...) CE_Debug("Registered system: %s (Type: %d, Run Order: %d, Run Phase: %d, Run Frequency: %d)", #name, name, name##_runOrder, name##_runPhase, name##_runFrequency);
        CE_SYSTEM_DESC_CORE(X)
        CE_SYSTEM_DESC_ENGINE(X)
        #ifndef CE_CORE_TEST_MODE
        CE_SYSTEM_DESC_GAME(X)
        #endif
    #undef X
#endif

	// Initialize storage structure
	context->m_storage = (CE_ECS_MainStorage){ .m_initialized = false };

    // Initialize main storage
    if (CE_ECS_MainStorage_init(&context->m_storage, context, errorCode) != CE_OK) {
        return CE_ERROR;
    }

    // Initialize runtime data
    // Reserve memory
    for (uint32_t order = 0; order < CE_ECS_SYSTEM_RUN_ORDER_COUNT; order++) {
        for (uint32_t freq = 0; freq < CE_ECS_SYSTEM_RUN_FREQUENCY_COUNT; freq++) {
            for (uint32_t phase = 0; phase < CE_ECS_SYSTEM_RUN_PHASE_COUNT; phase++) {
                CE_ECS_System_CacheList* cacheList = &context->m_systemRuntimeData.m_systemsByRunOrder[order].m_frequency[freq].m_phase[phase];
                cc_init(&cacheList->m_systems);
                if (!cc_reserve(&cacheList->m_systems, systemCount[order][freq][phase])) {
                    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
                    return CE_ERROR;
                }
            }
        }
    }

    // Populate cached system lists
    for (CE_TypeId sysType = 0; sysType < CE_SYSTEM_TYPES_COUNT; sysType++) {
        const CE_ECS_SystemStaticData* sysData = &context->m_systemDefinitions[sysType];
        if (sysData->m_isValid) {
            CE_ECS_System_CacheList* cacheList = &context->m_systemRuntimeData.m_systemsByRunOrder[sysData->m_runOrder].m_frequency[sysData->m_runFrequency].m_phase[sysData->m_runPhase];
            if (cc_push(&cacheList->m_systems, sysType) == NULL) {
                CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
                return CE_ERROR;
            }
        }
    }

    // Initialize runtime data counters
    context->m_systemRuntimeData.m_timeSinceLastRun = 0.0f;
    context->m_systemRuntimeData.m_frameCounter = 0;

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
    CE_Result result;
    context->m_systemRuntimeData.m_timeSinceLastRun += deltaTime;
    context->m_systemRuntimeData.m_frameCounter++;

    const bool runOncePerSecond = context->m_systemRuntimeData.m_timeSinceLastRun >= 1.0f;

    for (int phase = CE_ECS_SYSTEM_RUN_PHASE_EARLY; phase < CE_ECS_SYSTEM_RUN_PHASE_COUNT; phase++)
    {
        // Display
        result = CE_ECS_RunSystems_AutoOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, errorCode);
        if (result != CE_OK) {
            return CE_ERROR;
        }
        result = CE_ECS_RunSystems_SceneOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, errorCode);
        if (result != CE_OK) {
            return CE_ERROR;
        }
        
        // Half Display
        if (context->m_systemRuntimeData.m_frameCounter % 2 == 0) {
            result = CE_ECS_RunSystems_AutoOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, errorCode);
            if (result != CE_OK) {
                return CE_ERROR;
            }
            result = CE_ECS_RunSystems_SceneOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, errorCode);
            if (result != CE_OK) {
                return CE_ERROR;
            }
        }

        // Once Per Second
        if (runOncePerSecond)
        {
            result = CE_ECS_RunSystems_AutoOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND, errorCode);
            if (result != CE_OK) {
                return CE_ERROR;
            }
            result = CE_ECS_RunSystems_SceneOrder(context, deltaTime, phase, CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND, errorCode);
            if (result != CE_OK) {
                return CE_ERROR;
            }
        }
    }

    if (runOncePerSecond) {
        context->m_systemRuntimeData.m_timeSinceLastRun = 0.0f;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}



