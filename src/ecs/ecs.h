//
//  ecs/ecs.h
//  Primary ECS include (single entry point for Corgo ECS).
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2025 Carlos Camacho.
//

#ifndef CORGO_ECS_ECS_H
#define CORGO_ECS_ECS_H

#include "types.h"
#include "components.h"
#include "core/storage.h"

// Main ECS Context.
// All ECS definitions and data is stored here. 

typedef struct CE_ECS_Context {
    CE_ComponentStaticData componentDefinitions[CE_COMPONENT_TYPE_COUNT];
    CE_ECS_MainStorage storage;
   
} CE_ECS_Context;

void CE_ECS_Init(OUT CE_ECS_Context* context);

#endif // CORGO_ECS_ECS_H