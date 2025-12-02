//
//  ecs/core/storage.h
//  Main include file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_STORAGE_H
#define CORGO_ECS_CORE_STORAGE_H

#include "../types.h"
#include "../components.h"

typedef struct CE_ECS_MainStorage {
    void *componentArrays[CE_COMPONENT_TYPE_COUNT];
} CE_ECS_MainStorage;

#endif // CORGO_ECS_CORE_STORAGE_H