//
//  ecs/core/storage.c
//  Main implementation file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdlib.h>

#include "engine/core/memory.h"

#include "storage.h"
#include "../ecs.h"

CE_Result CE_ECS_MainStorage_init(OUT CE_ECS_MainStorage *storage, IN const CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode)
{
    if (storage->m_initialized)  
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_ALREADY_INITIALIZED);
        return CE_ERROR;
    }

    size_t componentStorageSize = 0;

    for (int x = 0; x < CE_COMPONENT_TYPES_COUNT; x++) {
        const size_t componentSize = context->m_componentDefinitions[x].m_storageSizeOf;
        const uint32_t initialCapacity = context->m_componentDefinitions[x].m_initialCapacity;

        // Check component capacity vs bitfield size
        if (initialCapacity > CE_BITSET_MAX_BITS) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_BITSET_TOO_SMALL);
            return CE_ERROR;
        }

        // Initialize each component type storage
        storage->m_componentTypeStorage[x] = NULL;
        CE_ECS_ComponentStorage *storageEntry = CE_realloc(NULL, sizeof(CE_ECS_ComponentStorage));
        if (!storageEntry) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        storageEntry->m_typeId = (CE_TypeId)x;
        storageEntry->m_capacity = initialCapacity;
        storageEntry->m_count = 0;
        cc_init(&storageEntry->m_componentMetadata);
        if (!cc_reserve(&storageEntry->m_componentMetadata, initialCapacity))
        {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        CE_Result result = CE_Bitset_init(&storageEntry->m_componentIndexBitset, initialCapacity);
        if (result != CE_OK) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        // Initialize component data pool
        storageEntry->m_componentDataPool = CE_realloc(NULL, initialCapacity * componentSize);
        if (!storageEntry->m_componentDataPool) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        CE_Debug("Component data pool allocated at %p for type %s of size %u bytes", storageEntry->m_componentDataPool, CE_ECS_GetComponentTypeNameDebugStr(x), initialCapacity * componentSize);
        componentStorageSize += initialCapacity * componentSize;

        // Initialize component headers
        CE_ECS_ComponentStorageHeader header = { .m_isValid = false };
        for (uint32_t i = 0; i < initialCapacity; i++) {
            if (cc_push(&storageEntry->m_componentMetadata, header) == NULL)
            {
                CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
                return CE_ERROR;
            }
        }

        storage->m_componentTypeStorage[x] = storageEntry;
    }

    // Initialize entity storage
    for (uint32_t i=0; i < CE_MAX_ENTITIES; i++) {
        storage->m_entityStorage.m_entityDataArray[i].m_isValid = false;
        storage->m_entityStorage.m_entityDataArray[i].m_entityId = CE_INVALID_ID;
        CE_Bitset_init(&storage->m_entityStorage.m_entityDataArray[i].m_entityComponentBitset, CE_COMPONENT_TYPES_COUNT);
        CE_Bitset_init(&storage->m_entityStorage.m_entityDataArray[i].m_entityRelationshipBitset, CE_RELATIONSHIP_TYPES_COUNT);
        
        cc_init(&storage->m_entityStorage.m_entityDataArray[i].m_components);
        cc_init(&storage->m_entityStorage.m_entityDataArray[i].m_relationships);
    }

    cc_init(&storage->m_entityStorage.m_knownEntities);
    if (!cc_reserve(&storage->m_entityStorage.m_knownEntities, CE_MAX_ENTITIES/4)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    }

    storage->m_initialized = true;

    CE_Debug("ECS Main Storage initialized. Total component storage size: %u bytes", componentStorageSize);

	CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_cleanup(OUT CE_ECS_MainStorage* storage, IN const CE_ECS_Context *context,  OUT CE_ERROR_CODE *errorCode)
{
    CE_Result result = CE_OK;

    if (storage->m_initialized) {
        for (int x = 0; x < CE_COMPONENT_TYPES_COUNT; x++) {
            const CE_ECS_ComponentStaticData* componentStaticData = &context->m_componentDefinitions[x];
            CE_ECS_ComponentStorage* storageEntry = storage->m_componentTypeStorage[x];
            if (storageEntry) {
                // Properly cleanup all components,, most likely release resources
                for (size_t i = 0; i < storageEntry->m_capacity; i++) {
                    if (CE_Bitset_isBitSet(&storageEntry->m_componentIndexBitset, i)) {
                        // Call cleanup function for each valid component
                        void* componentPtr = CE_ECS_ComponentStorage_getComponentDataPointer(storageEntry, componentStaticData, i);
                        if (componentPtr) {
                            result = componentStaticData->m_cleanupFunction(componentPtr);
                            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
                        }
                    }
                }
                // Bulk free memory
                cc_cleanup(&storageEntry->m_componentMetadata);
                CE_free(storageEntry->m_componentDataPool);
                CE_free(storageEntry);
                storage->m_componentTypeStorage[x] = NULL;
            }
        }

        for (int i=0; i < CE_MAX_ENTITIES; i++) {
            cc_cleanup(&storage->m_entityStorage.m_entityDataArray[i].m_components);
            cc_cleanup(&storage->m_entityStorage.m_entityDataArray[i].m_relationships);
        }

        cc_cleanup(&storage->m_entityStorage.m_knownEntities);
        storage->m_initialized = false;
    }

    if (result == CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    }

    return result;
}

CE_Result CE_ECS_MainStorage_growStorageForComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    // Not supporting dynamic growth yet
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_UNSUPPORTED_OPERATION);
    return CE_ERROR;
}

void* CE_ECS_ComponentStorage_getComponentDataPointer(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN size_t index)
{
    if (index >= storage->m_capacity || !CE_Bitset_isBitSet(&storage->m_componentIndexBitset, index)) {
        return NULL;
    }
    return (uint8_t*)storage->m_componentDataPool + (index * componentStaticData->m_storageSizeOf);
}

void* CE_ECS_ComponentStorage_getComponentDataPointerById(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id)
{
    const size_t index = CE_Id_getUniqueId(id);
    return CE_ECS_ComponentStorage_getComponentDataPointer(storage, componentStaticData, index);
}

CE_Result CE_ECS_MainStorage_createComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT CE_Id* id, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_OK;

    if (storage->m_initialized == false) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_NOT_INITIALIZED);
        return CE_ERROR;
    }

    CE_ECS_ComponentStorage* componentStorage = storage->m_componentTypeStorage[componentStaticData->m_type];
    if (!componentStorage) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_NOT_INITIALIZED);
        return CE_ERROR;
    }

    if (componentStorage->m_count >= componentStorage->m_capacity) {
        // At capacity, need to grow
        result = CE_ECS_MainStorage_growStorageForComponent(storage, componentStaticData, errorCode);
        if (result != CE_OK) {
            return result;
        }
    }

    // Find the first available slot
    size_t index;
    for (index = 0; index < componentStorage->m_capacity; index++) {
        if (!CE_Bitset_isBitSet(&componentStorage->m_componentIndexBitset, index))
        {
            // Set this here to reserve the space
            CE_Bitset_setBit(&componentStorage->m_componentIndexBitset, index);
            break;
        }
    }
    
    if (index == componentStorage->m_capacity) {
        // No available slot found, should not happen due to previous checks
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    // Initialize component
    void* componentPtr = CE_ECS_ComponentStorage_getComponentDataPointer(componentStorage, componentStaticData, index);
    if (!componentPtr) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    result = componentStaticData->m_initFunction(componentPtr);

    if (result != CE_OK) {
        // Init failed, free the slot
        CE_Bitset_clearBit(&componentStorage->m_componentIndexBitset, index);
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_INIT_FAILED);
        return result;
    }

    // Set header and generate id
    CE_ECS_ComponentStorageHeader* header = cc_get(&componentStorage->m_componentMetadata, index);
    if (!header) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    header->m_isValid = true;
    componentStorage->m_count++;
    CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, componentStaticData->m_type, 0, (uint32_t)index, id);

    // If passed in a pointer for component data, set it
    if (componentData) {
        *componentData = componentPtr;
    }

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_destroyComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_Result result = CE_OK;

    if (storage->m_initialized == false) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_NOT_INITIALIZED);
        return CE_ERROR;
    }

    const CE_TypeId typeId = CE_Id_getComponentTypeId(id);

    CE_ECS_ComponentStorage* componentStorage = storage->m_componentTypeStorage[typeId];
    if (!componentStorage) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_NOT_INITIALIZED);
        return CE_ERROR;
    }

    const uint32_t index = CE_Id_getUniqueId(id);

    if (!CE_Bitset_isBitSet(&componentStorage->m_componentIndexBitset, index)) {
        return CE_OK; // Component already destroyed
    }

    void* componentPtr = CE_ECS_ComponentStorage_getComponentDataPointer(componentStorage, componentStaticData, index);
    if (!componentPtr) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_COMPONENT_ID);
        return CE_ERROR;
    }

    result = componentStaticData->m_cleanupFunction(componentPtr);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
    }

    // Continue cleanup even if cleanup function failed, we want to free memory
    CE_ECS_ComponentStorageHeader* header = cc_get(&componentStorage->m_componentMetadata, index);
    if (!header) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }
    header->m_isValid = false;
    CE_Bitset_clearBit(&componentStorage->m_componentIndexBitset, index);
    componentStorage->m_count--;

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_createEntity(INOUT CE_ECS_MainStorage* storage, OUT CE_Id* id, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (storage->m_initialized == false) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_NOT_INITIALIZED);
        return CE_ERROR;
    }

    if (storage->m_entityStorage.m_count >= CE_MAX_ENTITIES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_MAX_ENTITIES_REACHED);
        return CE_ERROR;
    }

    // Find the first available slot for a new entity
    size_t index;
    for (index = 0; index < CE_MAX_ENTITIES; index++) {
        if (!storage->m_entityStorage.m_entityDataArray[index].m_isValid) {
            break;
        }
    }

    if (index == CE_MAX_ENTITIES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_MAX_ENTITIES_REACHED);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = &storage->m_entityStorage.m_entityDataArray[index];
    
    uint8_t generation = 0;
    if (entityData->m_entityId != CE_INVALID_ID) {
        generation = CE_Id_getGeneration(entityData->m_entityId) + 1;
        if (generation > CE_MAX_GENERATION_COUNT) {
            generation = 0;
        }
    }
    
    CE_Id newId = CE_INVALID_ID;
    CE_Result result = CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, generation, (uint32_t)index, &newId);
    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INTERNAL_ERROR);
        return CE_ERROR;
    }

    entityData->m_entityId = newId;
    entityData->m_isValid = true;
    CE_Bitset_clear(&entityData->m_entityComponentBitset);
    CE_Bitset_clear(&entityData->m_entityRelationshipBitset);
    cc_clear(&entityData->m_components);
    cc_clear(&entityData->m_relationships);

    // Initialize component and relationship arrays with initial capacity to prevent reallocations
    if (!cc_reserve(&entityData->m_components, CE_INITIAL_ENTITY_COMPONENTS_CAPACITY)
        || !cc_reserve(&entityData->m_relationships, CE_INITIAL_ENTITY_RELATIONSHIPS_CAPACITY))
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    }

    if (cc_insert(&storage->m_entityStorage.m_knownEntities, newId) == NULL) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_OUT_OF_MEMORY);
        return CE_ERROR;
    }

    storage->m_entityStorage.m_count++;
    *id = entityData->m_entityId;

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_destroyEntity(INOUT CE_ECS_MainStorage* storage, IN CE_Id id, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (storage->m_initialized == false) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_NOT_INITIALIZED);
        return CE_ERROR;
    }

    const uint32_t index = CE_Id_getUniqueId(id);
    if (index >= CE_MAX_ENTITIES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = &storage->m_entityStorage.m_entityDataArray[index];
    if (!entityData->m_isValid) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_NOT_FOUND);
        return CE_ERROR;
    }

    // Detect if the passed id is stale (references a deleted entity)
    const uint32_t generation = CE_Id_getGeneration(entityData->m_entityId);
    if (generation != CE_Id_getGeneration(id)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STALE_ENTITY_ID);
        return CE_ERROR;
    }

    entityData->m_isValid = false;
    storage->m_entityStorage.m_count--;
    CE_Bitset_clear(&entityData->m_entityComponentBitset);
    CE_Bitset_clear(&entityData->m_entityRelationshipBitset);
    cc_clear(&entityData->m_components);
    cc_clear(&entityData->m_relationships);

    cc_erase(&storage->m_entityStorage.m_knownEntities, id);

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_getEntityData(INOUT CE_ECS_MainStorage* storage, IN CE_Id id, OUT CE_ECS_EntityData** outData, OUT_OPT CE_ERROR_CODE* errorCode)
{
    if (storage->m_initialized == false) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_NOT_INITIALIZED);
        return CE_ERROR;
    }

    const uint32_t index = CE_Id_getUniqueId(id);
    if (index >= CE_MAX_ENTITIES) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }

    CE_ECS_EntityData* entityData = &storage->m_entityStorage.m_entityDataArray[index];
    if (!entityData->m_isValid) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENTITY_NOT_FOUND);
        return CE_ERROR;
    }

    // Detect if the passed id is stale (references a deleted entity)
    const uint32_t generation = CE_Id_getGeneration(entityData->m_entityId);
    if (generation != CE_Id_getGeneration(id)) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STALE_ENTITY_ID);
        return CE_ERROR;
    }

    *outData = entityData;
        
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}