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
    CE_ECS_SYSTEM_RUN_PHASE_EARLY = 0,
    CE_ECS_SYSTEM_RUN_PHASE_DEFAULT = 1,
    CE_ECS_SYSTEM_RUN_PHASE_LATE = 2,
    CE_ECS_SYSTEM_RUN_PHASE_COUNT = 3,
} CE_ECS_SYSTEM_RUN_PHASE;

typedef enum CE_ECS_SYSTEM_RUN_FREQUENCY {
    CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY = 0,
    CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY = 1,
    CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND = 2,
    CE_ECS_SYSTEM_RUN_FREQUENCY_COUNT = 3,
} CE_ECS_SYSTEM_RUN_FREQUENCY;

struct CE_ECS_SystemStaticData {
    bool m_isValid;
    bool m_enabled;
    CE_TypeId m_systemId;
    CE_ECS_SYSTEM_RUN_ORDER m_runOrder;
    CE_ECS_SYSTEM_RUN_PHASE m_runPhase;
    CE_ECS_SYSTEM_RUN_FREQUENCY m_runFrequency;
    CE_Bitset m_requiredComponentBitset; // Bitset of required component types for this system
    CE_Bitset m_requiredRelationshipBitset; // Bitset of required relationship types for this system
    CE_Result (*m_runFunction)(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);
};

// The following 3 structures cache the systems by their metadata for faster iteration
// CE_ECS_SystemRuntimeData.m_systemsByRunOrder[RUN_ORDER].m_frequency[RUN_FREQUENCY].m_phase[RUN_PHASE].m_systems
typedef struct CE_ECS_System_CacheList {
    CE_TypeId_Vector m_systems;
} CE_ECS_System_CacheList;

typedef struct CE_ECS_System_CacheList_RunPhase {
    CE_ECS_System_CacheList m_phase[CE_ECS_SYSTEM_RUN_PHASE_COUNT]; // Cached systems per run phase
} CE_ECS_System_CacheList_RunPhase;

typedef struct CE_ECS_System_CacheList_Frequency {
    CE_ECS_System_CacheList_RunPhase m_frequency[CE_ECS_SYSTEM_RUN_FREQUENCY_COUNT]; // Cached systems per run frequency
} CE_ECS_System_CacheList_Frequency;

// Runtime data container for all system information
typedef struct CE_ECS_SystemRuntimeData {
    CE_ECS_System_CacheList_Frequency m_systemsByRunOrder[CE_ECS_SYSTEM_RUN_ORDER_COUNT]; // Cached systems per run order
    float m_timeSinceLastRun; // Time accumulator for systems that run once per second
    uint32_t m_frameCounter; // Frame counter to help with frequency calculations
} CE_ECS_SystemRuntimeData;

// Generate the systems declarations
#define CE_DECLARE_SYSTEM(name, run_order, run_phase, run_frequency, ...)\
static const CE_ECS_SYSTEM_RUN_ORDER name##_runOrder = run_order;\
static const CE_ECS_SYSTEM_RUN_PHASE name##_runPhase = run_phase;\
static const CE_ECS_SYSTEM_RUN_FREQUENCY name##_runFrequency = run_frequency;\
CE_Result name##_run(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);\
void name##_description(OUT CE_ECS_SystemStaticData *data);\

#define CE_DECLARE_GLOBAL_SYSTEM(name, run_phase, run_frequency)\
CE_Result name##_global_run(INOUT struct CE_ECS_Context* context, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);

// Called by the dependency list to setup variables
#define REQUIRE_COMPONENT(componentType, varName) \
    componentType##_StorageType* varName = NULL;\
    CE_Id varName##_Id = CE_INVALID_ID;\
    CE_ECS_GetComponentForSystem(context, entity, componentType, systemDesc, &varName##_Id, (void**)&varName, errorCode);\
    if (*errorCode != CE_ERROR_CODE_NONE || varName##_Id == CE_INVALID_ID || varName == NULL) {\
        CE_Error("Entity %u missing required component type" #componentType " for system", entity);\
        return CE_ERROR;\
    }\

// Called by the relationship list to setup variables
#define REQUIRE_RELATIONSHIP(relationshipType, varName) \
    CE_Id varName##_Id = CE_INVALID_ID;\
    CE_ECS_GetRelationshipForSystem(context, entity, relationshipType, systemDesc, &varName##_Id, errorCode);\
    if (*errorCode != CE_ERROR_CODE_NONE || varName##_Id == CE_INVALID_ID) {\
        CE_Error("Entity %u missing required component type" #relationshipType " for system", entity);\
        return CE_ERROR;\
    }\


// Functions used to define and implement a system
// Load components and set up local variables
#define CE_START_SYSTEM_IMPLEMENTATION(name, ...)\
CE_Result name##_run(INOUT struct CE_ECS_Context* context, const IN CE_ECS_SystemStaticData *systemDesc, const IN CE_Id entity, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)\
{\
    __VA_ARGS__ \
    \

// Cleanup and close function
#define CE_END_SYSTEM_IMPLEMENTATION \
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);\
    return CE_OK;\
} \

// Functions used to define and implement a global system
#define CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(name)\
CE_Result name##_global_run(INOUT struct CE_ECS_Context* context, const IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)\
{\

// Cleanup and close function
#define CE_END_GLOBAL_SYSTEM_IMPLEMENTATION \
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);\
    return CE_OK;\
} \

#endif // CORGO_ECS_CORE_SYSTEM_H