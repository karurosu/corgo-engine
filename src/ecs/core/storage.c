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
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_ALREADY_INITIALIZED);
        return CE_ERROR;
    }

    for (int x = 0; x < CE_COMPONENT_TYPES_COUNT; x++) {
        const size_t componentSize = context->m_componentDefinitions[x].m_storageSizeOf;
        const size_t initialCapacity = context->m_componentDefinitions[x].m_initialCapacity;

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

        // Initialize component data pool
        storageEntry->m_componentDataPool = calloc(initialCapacity, componentSize);
        if (!storageEntry->m_componentDataPool) {
            CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_STORAGE_COMPONENT_DATA_POOL_ALLOCATION_FAILED);
            return CE_ERROR;
        }

        // Initialize component headers
        for (int i = 0; i < initialCapacity; i++) {
            storageEntry->m_componentHeader[i].isValid = false;
        }

        storage->m_componentTypeStorage[x] = storageEntry;
    }

    storage->m_initialized = true;
	CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}

CE_Result CE_ECS_MainStorage_cleanup(OUT CE_ECS_MainStorage* storage, OUT CE_ERROR_CODE *errorCode)
{
    if (storage->m_initialized) {
        for (int x = 0; x < CE_COMPONENT_TYPES_COUNT; x++) {
            CE_ECS_ComponentStorage* storageEntry = storage->m_componentTypeStorage[x];
            if (storageEntry) {
                free(storageEntry->m_componentDataPool);
                free(storageEntry);
                storage->m_componentTypeStorage[x] = NULL;
            }
        }

        storage->m_initialized = false;
    }

	CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}