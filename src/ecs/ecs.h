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
    CE_ECS_ComponentStaticData m_componentDefinitions[CE_COMPONENT_TYPE_COUNT];
    CE_ECS_MainStorage m_storage;
};

CE_Result CE_ECS_Init(OUT CE_ECS_Context* context, OUT CE_ERROR_CODE *errorCode);
CE_Result CE_ECS_Cleanup(OUT CE_ECS_Context* context, OUT CE_ERROR_CODE* errorCode);

#endif // CORGO_ECS_ECS_H