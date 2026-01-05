//
//  ecs/core/storage.c
//  Main implementation file for all data storage definitions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdlib.h>

#include "storage.h"
#include "../ecs.h"

CE_Result CE_ECS_MainStorage_init(OUT CE_ECS_MainStorage *storage, IN const CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode)
{
    if (storage->m_initialized)  
    {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_ALREADY_INITIALIZED);
        return CE_ERROR;
    }

    for (int x = 0; x < CE_COMPONENT_TYPES_COUNT; x++) {
        const size_t componentSize = context->m_componentDefinitions[x].m_storageSizeOf;
        const size_t initialCapacity = context->m_componentDefinitions[x].m_initialCapacity;

        // Check component capacity vs bitfield size
        if (initialCapacity > CE_BITSET_MAX_BITS) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_BITSET_TOO_SMALL);
            return CE_ERROR;
        }

        // Initialize each component type storage
        storage->m_componentTypeStorage[x] = NULL;
        CE_ECS_ComponentStorage *storageEntry = malloc(sizeof(CE_ECS_ComponentStorage) + sizeof(CE_ECS_ComponentStorageHeader) * initialCapacity);
        if (!storageEntry) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_MAIN_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        storageEntry->m_typeId = (CE_TypeId)x;
        storageEntry->m_capacity = initialCapacity;
        storageEntry->m_count = 0;

        CE_Result result = CE_Bitset_init(&storageEntry->m_componentIndexBitset, initialCapacity);
        if (result != CE_OK) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        // Initialize component data pool
        storageEntry->m_componentDataPool = calloc(initialCapacity, componentSize);
        if (!storageEntry->m_componentDataPool) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        // Initialize component headers
        for (int i = 0; i < initialCapacity; i++) {
            storageEntry->m_componentHeader[i].m_isValid = false;
        }

        storage->m_componentTypeStorage[x] = storageEntry;
    }

    storage->m_initialized = true;
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
                        void* componentPtr = CE_ECS_MainStorage_getComponentDataPointer(storageEntry, componentStaticData, i);
                        if (componentPtr) {
                            result = componentStaticData->m_cleanupFunction(componentPtr);
                            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
                        }
                    }
                }
                
                // Bulk free memory
                free(storageEntry->m_componentDataPool);
                free(storageEntry);
                storage->m_componentTypeStorage[x] = NULL;
            }
        }

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

void* CE_ECS_MainStorage_getComponentDataPointer(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN size_t index)
{
    if (index >= storage->m_capacity || !CE_Bitset_isBitSet(&storage->m_componentIndexBitset, index)) {
        return NULL;
    }
    return (uint8_t*)storage->m_componentDataPool + (index * componentStaticData->m_storageSizeOf);
}

void* CE_ECS_MainStorage_getComponentDataPointerById(INOUT CE_ECS_ComponentStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, IN CE_Id id)
{
    const size_t index = CE_Id_getUniqueId(id);
    return CE_ECS_MainStorage_getComponentDataPointer(storage, componentStaticData, index);
}

CE_Result CE_ECS_MainStorage_createComponent(INOUT CE_ECS_MainStorage* storage, IN const CE_ECS_ComponentStaticData *componentStaticData, OUT CE_Id* id, OUT_OPT CE_ERROR_CODE* errorCode)
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
            CE_Bitset_setBit(&componentStorage->m_componentIndexBitset, index);
            componentStorage->m_componentHeader[index].m_isValid = true;
            componentStorage->m_count++;
            break;
        }
    }
    
    if (index == componentStorage->m_capacity) {
        // No available slot found, should not happen due to previous checks
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
        return CE_ERROR;
    }

    // Initialize component
    void* componentPtr = CE_ECS_MainStorage_getComponentDataPointer(componentStorage, componentStaticData, index);
    if (!componentPtr) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
        return CE_ERROR;
    }

    result = componentStaticData->m_initFunction(componentPtr);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_INIT_FAILED);
        return result;
    }

    // Set header and generate id
    componentStorage->m_componentHeader[index].m_isValid = true;
    componentStorage->m_count++;
    CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, componentStaticData->m_type, 0, (uint32_t)index, id);

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

    void* componentPtr = CE_ECS_MainStorage_getComponentDataPointer(componentStorage, componentStaticData, index);
    if (!componentPtr) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_ALLOCATION_FAILED);
        return CE_ERROR;
    }

    result = componentStaticData->m_cleanupFunction(componentPtr);

    if (result != CE_OK) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_CLEANUP_FAILED);
    }

    // Call this anyway since the old component is probably corrupted, data may have leaked but whatever
    CE_Bitset_clearBit(&componentStorage->m_componentIndexBitset, index);
    componentStorage->m_componentHeader[index].m_isValid = false;
    componentStorage->m_count--;

    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}
