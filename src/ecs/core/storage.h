//
//  ecs/core/storage.h
//  Main include file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_STORAGE_H
#define CORGO_ECS_CORE_STORAGE_H

#include "../types.h"
#include "../components.h"

// Component storage structures
typedef struct CE_ECS_ComponentStorageHeader {
    bool m_isValid;
} CE_ECS_ComponentStorageHeader;

typedef struct CE_ECS_ComponentStorage {
    CE_TypeId m_typeId;
    size_t m_capacity;
    size_t m_count;
    void *m_componentDataPool;
    CE_Bitset m_componentIndexBitset; // Bitset to track used indices                   
    CE_ECS_ComponentStorageHeader m_componentHeader[];
} CE_ECS_ComponentStorage;

// Entity storage definitions
typedef struct CE_ECS_EntityData {
    bool m_isValid;
    CE_Id m_entityId;
    CE_Bitset m_entityComponentBitset; // Bitset to track which components are attached to the entity
    CE_Id_Vector m_components; // Vector of component ids attached to the entity
    CE_Id_Vector m_relationships; // Vector of relationship ids attached to the entity
} CE_ECS_EntityData;

typedef struct CE_ECS_EntityStorage {
    size_t m_count;
    CE_ECS_EntityData m_entityDataArray[CE_MAX_ENTITIES];
} CE_ECS_EntityStorage;

// Main storage structure
typedef struct CE_ECS_MainStorage {
    bool m_initialized;
    CE_ECS_ComponentStorage *m_componentTypeStorage[CE_COMPONENT_TYPES_COUNT]; // Array of component storages indexed by component type
    CE_ECS_EntityStorage m_entityStorage;
} CE_ECS_MainStorage;

// Initialization and cleanup functions
CE_Result CE_ECS_MainStorage_init(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_Context *context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_cleanup(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_Context *context, OUT_OPT CE_ERROR_CODE *errorCode);

// Component creation and management functions
CE_Result CE_ECS_MainStorage_createComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT CE_Id* id, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_destroyComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_growStorageForComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT_OPT CE_ERROR_CODE* errorCode);

// Component data access
void* CE_ECS_MainStorage_getComponentDataPointerById(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id);
void* CE_ECS_MainStorage_getComponentDataPointer(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN size_t index);



#endif // CORGO_ECS_CORE_STORAGE_H