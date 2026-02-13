//
//  ecs/core/ecs_internal.c
//  ECS functions for internal use only.
//  Engine or user code should not use this header.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "ecs_internal.h"
#include "ecs/ecs.h"

#include "system.h"

CE_Result CE_ECS_RunSystems_AutoOrder(INOUT CE_ECS_Context* context, IN float deltaTime, CE_ECS_SYSTEM_RUN_PHASE phase, CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode)
{
    const CE_ECS_System_CacheList *systemList = &context->m_systemRuntimeData.m_systemsByRunOrder[CE_ECS_SYSTEM_RUN_ORDER_AUTO].m_frequency[frequency].m_phase[phase];

    if (cc_size(&systemList->m_systems) == 0) {
        // No systems to run
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
        return CE_OK;
    }
    // TODO: optimize entity order for cache access
    for (int entityIndex = 0; entityIndex < CE_MAX_ENTITIES; entityIndex++) 
    {
        if (!CE_Bitset_isBitSet(&context->m_storage.m_entityStorage.m_entityIndexBitset, entityIndex)) {
            continue; // Skip unused entity slots
        }

        // Since we are doing direct iteration we don't need to do as many checks for getting entity data
        CE_ECS_EntityData* entityData = CE_ECS_MainStorage_getEntityDataDirectly(&context->m_storage, entityIndex);
        
        // Run each system in the cached list
        cc_for_each(&systemList->m_systems, sysTypeIdPtr) 
        {
            // Error handling is done inside the function
            // We just continue to the next system on failure
            CE_ECS_RunSystemOnEntity(context, deltaTime, *sysTypeIdPtr, entityData);
        }
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_RunSystems_SceneOrder(INOUT CE_ECS_Context* context, IN float deltaTime, CE_ECS_SYSTEM_RUN_PHASE phase, CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_RunSystemOnEntity(INOUT CE_ECS_Context* context, IN float deltaTime, IN CE_TypeId systemTypeId, IN const CE_ECS_EntityData *entityData)
{
    CE_Result result = CE_ERROR;
    CE_ERROR_CODE localErrorCode = CE_ERROR_CODE_NONE;
    const CE_ECS_SystemStaticData* sysData = &context->m_systemDefinitions[systemTypeId];
    
    if (!sysData->m_isValid || !sysData->m_enabled) {
        return CE_OK; // Skip invalid or disabled systems
    }

    // Check if entity matches system requirements
    if (!CE_Bitset_containsBits(&entityData->m_entityComponentBitset, &sysData->m_requiredComponentBitset) ||
        !CE_Bitset_containsBits(&entityData->m_entityRelationshipBitset, &sysData->m_requiredRelationshipBitset)) {
        return CE_OK; // Entity does not match requirements
    }

    // Run the system function
    result = sysData->m_runFunction(context, sysData, entityData->m_entityId, deltaTime, &localErrorCode);
    if (result != CE_OK) {
        CE_Error("System with Type ID %d failed to run on entity %d with error code %s", CE_ECS_GetSystemTypeNameDebugStr(systemTypeId), entityData->m_entityId, CE_GetErrorMessage(localErrorCode));
        return CE_ERROR;
    }
    return CE_OK;
}

#define GENERATE_RUN_GLOBAL_SYSTEM_CASE(name, run_phase, run_frequency, exp_run_order, exp_run_phase, exp_run_frequency) \
if (run_phase == exp_run_phase && run_frequency == exp_run_frequency) {\
    result = name##_global_run(context, deltaTime, errorCode);\
    if (result != CE_OK) {\
        CE_Error("Global system " #name " failed to run with error code %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }\
}

// Helper to run global systems based on the requested order, phase and frequency.
// it will run all the systems that match the criteria.
CE_Result CE_ECS_RunGlobalSystems(INOUT CE_ECS_Context* context, IN float deltaTime, IN CE_ECS_SYSTEM_RUN_PHASE phase, IN CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_ERROR;

#define X(name, run_phase, run_frequency) GENERATE_RUN_GLOBAL_SYSTEM_CASE(name, run_phase, run_frequency, runOrder, phase, frequency)
	CE_GLOBAL_SYSTEM_DESC_CORE(X)
	CE_GLOBAL_SYSTEM_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_GLOBAL_SYSTEM_DESC_GAME(X)
#endif
#undef X

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_RunSystems_RenderOrder(INOUT CE_ECS_Context* context, IN float deltaTime, IN CE_ECS_SYSTEM_RUN_PHASE phase, IN CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_ERROR;
    const CE_ECS_System_CacheList *systemList = &context->m_systemRuntimeData.m_systemsByRunOrder[CE_ECS_SYSTEM_RUN_ORDER_RENDER].m_frequency[frequency].m_phase[phase];

    if (cc_size(&systemList->m_systems) == 0) {
        // No systems to run
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
        return CE_OK;
    }

    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_SCENE_GRAPH_COMPONENT, sceneGraph);
    
    cc_for_each(&sceneGraph->m_renderList, index, renderNodePtr)
    {
        CE_ECS_EntityData* entityData;

        result = CE_ECS_MainStorage_getEntityDataByUniqueId(&context->m_storage, *index, &entityData, errorCode);
        if (entityData == NULL || result != CE_OK) {
            return CE_ERROR;
        }

        // Run each system in the cached list
        cc_for_each(&systemList->m_systems, sysTypeIdPtr) 
        {
            CE_ECS_RunSystemOnEntity(context, deltaTime, *sysTypeIdPtr, entityData);
        }
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

