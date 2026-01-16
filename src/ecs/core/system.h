//
//  ecs/core/system.h
//  Main include file for all systems definitions.
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

typedef struct CE_ECS_SystemStaticData {
    CE_TypeId m_systemId;
    CE_ECS_SYSTEM_RUN_ORDER m_runOrder;
    CE_ECS_SYSTEM_RUN_PHASE m_runPhase;
    CE_ECS_SYSTEM_RUN_FREQUENCY m_runFrequency;
    CE_Bitset m_requiredComponentBitset; // Bitset of required component types for this system
    CE_Bitset m_requiredRelationshipBitset; // Bitset of required relationship types for this system
    void (*m_runFunction)(INOUT struct CE_ECS_Context* context, float deltaTime);
} CE_ECS_SystemStaticData;

#endif // CORGO_ECS_CORE_SYSTEM_H