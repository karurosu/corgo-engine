//
//  ecs/core/ecs_internal.h
//  ECS functions for internal use only.
//  Engine or user code should not use this header.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ECS_INTERNAL_H
#define CORGO_ECS_CORE_ECS_INTERNAL_H

#include "engine/core/platform.h"
#include "ecs/types.h"
#include "storage.h"
#include "context.h"

// Helpers to run systems on different orders
CE_Result CE_ECS_RunSystems_AutoOrder(INOUT CE_ECS_Context* context, IN float deltaTime, CE_ECS_SYSTEM_RUN_PHASE phase, CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_RunSystems_SceneOrder(INOUT CE_ECS_Context* context, IN float deltaTime, CE_ECS_SYSTEM_RUN_PHASE phase, CE_ECS_SYSTEM_RUN_FREQUENCY frequency, OUT_OPT CE_ERROR_CODE* errorCode);

CE_Result CE_ECS_RunSystemOnEntity(INOUT CE_ECS_Context* context, IN float deltaTime, IN CE_TypeId systemTypeId, IN const CE_ECS_EntityData *entityData);


#endif // CORGO_ECS_CORE_ECS_INTERNAL_H
