//
//  ecs/ecs.h
//  Primary ECS include (single entry point for Corgo ECS).
//  This is the public API for Corgo ECS.
//  Copyright (c) 2025 Carlos Camacho.
//

#ifndef CORGO_ECS_ECS_H
#define CORGO_ECS_ECS_H

#include "types.h"
#include "core/id.h"
#include "components.h"
#include "core/storage.h"

// Main ECS Functions
CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE *errorCode);
CE_Result CE_ECS_Cleanup(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_Tick(INOUT CE_ECS_Context* context, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);

// Creation Functions
CE_Result CE_ECS_CreateEntity(INOUT CE_ECS_Context* context, OUT CE_Id* outId, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_DestroyEntity(INOUT CE_ECS_Context* context, IN CE_Id entity, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode);

// Entity data access functions
#include "core/entity.h"

///// Component data access functions
CE_Result CE_ECS_GetComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_Id componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_FindFirstComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_FindAllComponents(INOUT CE_ECS_Context* context, IN CE_Id entity, CE_TypeId componentType, OUT CE_Id *results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode);

#endif // CORGO_ECS_ECS_H