//
//  ecs/ecs.h
//  Primary ECS include (single entry point for Corgo ECS).
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2025 Carlos Camacho.
//

#ifndef CORGO_ECS_ECS_H
#define CORGO_ECS_ECS_H

#include "types.h"
#include "core/id.h"
#include "components.h"
#include "core/storage.h"

// Main ECS Context.
// All ECS definitions and data is stored here. 

struct CE_ECS_Context {
    // Static component definitions
    CE_ECS_ComponentStaticData m_componentDefinitions[CE_COMPONENT_TYPES_COUNT];
    
    // Main storage for ECS entities and components
    CE_ECS_MainStorage m_storage;
};

// Main ECS Functions
CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE *errorCode);
CE_Result CE_ECS_Cleanup(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_Tick(INOUT CE_ECS_Context* context, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);

// Creation Functions
CE_Result CE_ECS_CreateEntity(INOUT CE_ECS_Context* context, OUT CE_Id* outId, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_DestroyEntity(INOUT CE_ECS_Context* context, IN CE_Id entity, OUT_OPT CE_ERROR_CODE* errorCode);

// Entity data access functions
#include "core/entity.h"


#endif // CORGO_ECS_ECS_H