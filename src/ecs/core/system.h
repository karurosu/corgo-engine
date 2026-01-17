//
//  ecs/core/system.h
//  Main include file for all system related definitions.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_SYSTEM_H
#define CORGO_ECS_CORE_SYSTEM_H

#include "../types.h"

typedef enum CE_ECS_SYSTEM_RUN_ORDER {
    CE_ECS_SYSTEM_RUN_ORDER_AUTO = 0,
    CE_ECS_SYSTEM_RUN_ORDER_SCENETREE = 1,
    CE_ECS_SYSTEM_RUN_ORDER_COUNT = 2,
} CE_ECS_SYSTEM_RUN_ORDER;

typedef enum CE_ECS_SYSTEM_RUN_PHASE {
    CE_ECS_SYSTEM_RUN_PHASE_DEFAULT = 0,
    CE_ECS_SYSTEM_RUN_PHASE_EARLY = 1,
    CE_ECS_SYSTEM_RUN_PHASE_LATE = 2,
    CE_ECS_SYSTEM_RUN_PHASE_LAST = 3,
    CE_ECS_SYSTEM_RUN_PHASE_COUNT = 4,
} CE_ECS_SYSTEM_RUN_PHASE;

typedef enum CE_ECS_SYSTEM_RUN_FREQUENCY {
    CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY = 0,
    CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY = 1,
    CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND = 2,
    CE_ECS_SYSTEM_RUN_FREQUENCY_COUNT = 3,
} CE_ECS_SYSTEM_RUN_FREQUENCY;

struct CE_ECS_SystemStaticData {
    CE_TypeId m_systemId;
    CE_ECS_SYSTEM_RUN_ORDER m_runOrder;
    CE_ECS_SYSTEM_RUN_PHASE m_runPhase;
    CE_ECS_SYSTEM_RUN_FREQUENCY m_runFrequency;
    CE_Bitset m_requiredComponentBitset; // Bitset of required component types for this system
    CE_Bitset m_requiredRelationshipBitset; // Bitset of required relationship types for this system
    CE_Result (*m_runFunction)(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);
};

// Generate the systems declarations, note that run_with_params has the components first to solve a token replacement problem with variadic macros.
#define CE_DECLARE_SYSTEM(name, run_order, run_phase, run_frequency, ...)\
static const CE_ECS_SYSTEM_RUN_ORDER name##_runOrder = run_order;\
static const CE_ECS_SYSTEM_RUN_PHASE name##_runPhase = run_phase;\
static const CE_ECS_SYSTEM_RUN_FREQUENCY name##_runFrequency = run_frequency;\
CE_Result name##_run(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);\
void name##_description(OUT CE_ECS_SystemStaticData *data);\


// Called by the dependency list to setup variables
#define REQUIRE_COMPONENT(componentType, varName) \
    componentType##_StorageType* varName = NULL;\
    CE_Id varName##_Id = CE_INVALID_ID;\
    CE_ECS_GetComponentForSystem(context, entity, componentType, systemDesc, &varName##_Id, &varName, errorCode);\
    if (*errorCode != CE_ERROR_CODE_NONE || varName##_Id == CE_INVALID_ID || varName == NULL) {\
        CE_Error("Entity %u missing required component type" #componentType " for system", entity);\
        return CE_ERROR;\
    }\

// Load components and set up local variables
#define CE_START_SYSTEM_IMPLEMENTATION(name, ...)\
CE_Result name##_run(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)\
{\
    __VA_ARGS__ \
    \

// Cleanup and end function
#define CE_END_SYSTEM_IMPLEMENTATION \
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);\
    return CE_OK;\
} \

#endif // CORGO_ECS_CORE_SYSTEM_H