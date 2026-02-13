//
//  ecs/core/storage.h
//  Main include file for all data storage definitions.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_STORAGE_H
#define CORGO_ECS_CORE_STORAGE_H

#include "../types.h"
#include "entity.h"
#include "../components.h"
#include "../systems.h"

// Component storage structures
// Component header is used to track metadata for each component instance in storage
typedef struct CE_ECS_ComponentStorageHeader {
    bool m_isValid;
} CE_ECS_ComponentStorageHeader;

typedef struct CE_ECS_ComponentStorage {
    CE_TypeId m_typeId; // Type ID of the component
    uint16_t m_capacity; // Total capacity of the storage for this component type
    uint16_t m_count; // Number of currently alive components of this type
    void *m_componentDataPool; // Contiguous block of memory for component data, indexed by component unique ID
    CE_Bitset m_componentIndexBitset; // Bitset to track used indices
    cc_vec(CE_ECS_ComponentStorageHeader) m_componentMetadata; // Metadata for each component instance
} CE_ECS_ComponentStorage;

typedef struct CE_ECS_EntityStorage {
    uint16_t m_count; // Number of currently alive entities
    CE_Bitset m_entityIndexBitset; // Bitset to track used indices
    CE_ECS_EntityData m_entityDataArray[CE_MAX_ENTITIES]; // Fixed-size array for entity data, indexed by entity unique ID
} CE_ECS_EntityStorage;

// Global component storage definitions
typedef struct CE_ECS_GlobalComponentStorage {
#define X(name, storage) storage CE_PASTE(m_, CE_GLOBAL_COMPONENT(name));
	CE_GLOBAL_COMPONENT_DESC_CORE(X)
	CE_GLOBAL_COMPONENT_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_GLOBAL_COMPONENT_DESC_GAME(X)
#endif
#undef X
} CE_ECS_GlobalComponentStorage;

// Main storage structure
typedef struct CE_ECS_MainStorage {
    bool m_initialized;
    CE_ECS_ComponentStorage *m_componentTypeStorage[CE_COMPONENT_TYPES_COUNT]; // Array of component storages indexed by component type
    CE_ECS_EntityStorage m_entityStorage;
    CE_ECS_GlobalComponentStorage m_globalComponents;
} CE_ECS_MainStorage;

// Initialization and cleanup functions
CE_Result CE_ECS_MainStorage_init(INOUT CE_ECS_MainStorage* storage, IN CE_ECS_Context *context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_cleanup(INOUT CE_ECS_MainStorage* storage, IN CE_ECS_Context *context, OUT_OPT CE_ERROR_CODE *errorCode);

// Component creation and management functions
CE_Result CE_ECS_MainStorage_createComponent(INOUT CE_ECS_MainStorage* storage, IN CE_ECS_Context *context, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT CE_Id* id, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_destroyComponent(INOUT CE_ECS_MainStorage* storage, IN CE_ECS_Context *context, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_growStorageForComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT_OPT CE_ERROR_CODE* errorCode);

// Component data access
void* CE_ECS_ComponentStorage_getComponentDataPointerById(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id);
void* CE_ECS_ComponentStorage_getComponentDataPointer(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN uint16_t index);

// Entity creation and management functions
CE_Result CE_ECS_MainStorage_createEntity(INOUT CE_ECS_MainStorage* storage, OUT CE_Id* id, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_destroyEntity(INOUT CE_ECS_MainStorage* storage, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode);

// Entity access functions
CE_Result CE_ECS_MainStorage_getEntityData(INOUT CE_ECS_MainStorage* storage, IN CE_Id id, OUT CE_ECS_EntityData** outData, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_ECS_MainStorage_getEntityDataByUniqueId(INOUT CE_ECS_MainStorage* storage, IN uint16_t uniqueId, OUT CE_ECS_EntityData** outData, OUT_OPT CE_ERROR_CODE* errorCode);

// Helper function to directly get entity data without error checking (for internal use)
inline CE_ECS_EntityData* CE_ECS_MainStorage_getEntityDataDirectly(INOUT CE_ECS_MainStorage* storage, IN uint16_t id) {
    return &(storage->m_entityStorage.m_entityDataArray[id]);
}

#endif // CORGO_ECS_CORE_STORAGE_H