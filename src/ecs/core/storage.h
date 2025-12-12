//
//  ecs/core/storage.h
//  Main include file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_STORAGE_H
#define CORGO_ECS_CORE_STORAGE_H

#include "../types.h"
#include "../components.h"

typedef struct CE_ECS_ComponentStorageHeader {
    bool isValid;
} CE_ECS_ComponentStorageHeader;

typedef struct CE_ECS_ComponentStorage {
    CE_TypeId m_typeId;
    size_t m_capacity;
    size_t m_count;
    void *m_componentDataPool;
    CE_ECS_ComponentStorageHeader m_componentHeader[];
} CE_ECS_ComponentStorage;

typedef struct CE_ECS_MainStorage {
    bool m_initialized;
    CE_ECS_ComponentStorage *m_componentTypeStorage[CE_COMPONENT_TYPES_COUNT]; // Array of component storages indexed by component type
} CE_ECS_MainStorage;

// Initialization and cleanup functions
CE_Result CE_ECS_MainStorage_init(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_Context *context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_cleanup(INOUT CE_ECS_MainStorage* storage, OUT_OPT CE_ERROR_CODE *errorCode);

// Component creation and management functions
CE_Result CE_ECS_MainStorage_createComponent(INOUT CE_ECS_MainStorage* storage, IN CE_TypeId typeId, OUT CE_Id* id, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_destroyComponent(INOUT CE_ECS_MainStorage* storage, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode);

#endif // CORGO_ECS_CORE_STORAGE_H