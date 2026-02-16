//
//  ecs/core/context.h
//  Main header file for the ECS context structure.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_CONTEXT_H
#define CORGO_ECS_CORE_CONTEXT_H

#include "../types.h"
#include "storage.h"
#include "system.h"

typedef struct CE_ECS_CallingContext {
    CE_Id m_currentEntity;
    CE_Id m_currentComponent;
} CE_ECS_CallingContext;

// Main ECS Context
struct CE_ECS_Context {
    // Static component definitions
    CE_ECS_ComponentStaticData m_componentDefinitions[CE_COMPONENT_TYPES_COUNT];
    CE_ECS_SystemStaticData m_systemDefinitions[CE_SYSTEM_TYPES_COUNT];
    
    // Main storage for ECS entities and components
    CE_ECS_MainStorage m_storage;

    // Runtime data
    CE_ECS_SystemRuntimeData m_systemRuntimeData;

    // Calling context, used for systems to query which context they are running in
    CE_ECS_CallingContext m_callingContext;
};

#endif // CORGO_ECS_CORE_CONTEXT_H