//
//  ecs/core/storage.h
//  Main include file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_STORAGE_H
#define CORGO_ECS_CORE_STORAGE_H

#include "../types.h"
#include "../components.h"

#define CE_INITIAL_COMPONENT_CAPACITY 64
#define CE_COMPONENT_GROWTH_AMOUNT 32

typedef struct CE_ECS_ComponentStorageHeader {
    bool isValid;
} CE_ECS_ComponentStorageHeader;

typedef struct CE_ECS_ComponentStorage {
    CE_TypeId m_typeId;
    uint16_t m_capacity;
    uint16_t m_count;
    void *m_componentDataPool;
    CE_ECS_ComponentStorageHeader m_componentHeader[];
} CE_ECS_ComponentStorage;

typedef struct CE_ECS_MainStorage {
    bool m_initialized;
    CE_ECS_ComponentStorage *m_componentTypeStorage[CE_COMPONENT_TYPE_COUNT]; // Array of component storages indexed by component type
} CE_ECS_MainStorage;

CE_Result CE_ECS_MainStorage_init(OUT CE_ECS_MainStorage* storage, IN const CE_ECS_Context *context, OUT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_cleanup(OUT CE_ECS_MainStorage* storage, OUT CE_ERROR_CODE *errorCode);

#endif // CORGO_ECS_CORE_STORAGE_H