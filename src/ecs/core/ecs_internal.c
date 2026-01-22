//
//  ecs/core/ecs_internal.c
//  ECS functions for internal use only.
//  Engine or user code should not use this header.
//  Copyright (c) 2026 Carlos Camacho.
//

#include "ecs_internal.h"

#include "system.h"

CE_Result CE_ECS_RunSystems_AutoOrder(INOUT CE_ECS_Context* context, IN float deltaTime, CE_ECS_SYSTEM_RUN_PHASE phase, CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_System_CacheList *systemList = &context->m_systemRuntimeData.m_systemsByRunOrder[CE_ECS_SYSTEM_RUN_ORDER_AUTO].m_frequency[frequency].m_phase[phase];

    // TODO: optimize entity order and cache for systems
    cc_for_each(&context->m_storage.m_entityStorage.m_knownEntities, entityIdPtr) 
    {
        const CE_Id entityId = *entityIdPtr;

        // Run each system in the cached list
        cc_for_each(&systemList->m_systems, sysTypeIdPtr) 
        {
            const CE_TypeId sysTypeId = *sysTypeIdPtr;
            const CE_ECS_SystemStaticData* sysData = &context->m_systemDefinitions[sysTypeId];

            // Run the system function
            CE_Result result = sysData->m_runFunction(context, sysData, entityId, deltaTime, errorCode);
            if (result != CE_OK) {
                CE_Error("System with Type ID %d failed to run on entity %d", CE_ECS_GetSystemTypeNameDebugStr(sysTypeId), entityId);
                return CE_ERROR;
            }
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
