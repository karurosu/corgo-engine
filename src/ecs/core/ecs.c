//
//  ecs/ecs.c
//  Primary ECS C file, header is ecs/ecs.h
//  This file is in /ecs/core to keep only header files at the root of ecs/
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2025 Carlos Camacho.
//

#include "../ecs.h"

void CE_ECS_Init(OUT CE_ECS_Context* context)
{
    // Clear all component definitions
    for (uint32_t i = 0; i < CE_COMPONENT_TYPE_COUNT; i++) {
        context->componentDefinitions[i] = (CE_ComponentStaticData){ .bitmask = 0, .isValid = false, .type = 0, .uid = 0 };
    }

    // Gather component descriptions into the context
#define X(name, uid, storage) name##_description(&context->componentDefinitions[name]);
    CE_COMPONENT_DESC_CORE(X)
    #ifndef CE_CORE_TEST_MODE
    CE_COMPONENT_DESC_ENGINE(X)
    CE_COMPONENT_DESC_GAME(X)
    #endif
#undef X
}