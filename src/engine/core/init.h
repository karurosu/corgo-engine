//
//  engine/core/init.h
//  Engine initialization functions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_INIT_H
#define CORGO_ENGINE_CORE_INIT_H

#include "ecs/types.h"

// Internal functions for engine initialization and shutdown. These are not exposed in the public API and are only used by the engine core and backends.
CE_Result CE_Engine_Init(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode);
CE_Result CE_Engine_Shutdown(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode);
CE_Result CE_Engine_Tick(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode);


#endif // CORGO_ENGINE_CORE_INIT_H
