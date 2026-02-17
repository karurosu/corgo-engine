//
//  engine/core/asset_cache.c
//  A simple refcounted asset cache.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "asset_cache.h"
#include "engine/corgo.h"

// Component functions
CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_ASSET_CACHE)
{
    cc_init(&component->m_assetCount);
    if (!cc_reserve(&component->m_assetCount, CE_ENGINE_ASSET_CACHE_INITIAL_SIZE))
    {
        CE_Error("Failed to allocate asset cache");
        return CE_ERROR;
    }

    cc_init(&component->m_assetData);
    if (!cc_reserve(&component->m_assetData, CE_ENGINE_ASSET_CACHE_INITIAL_SIZE))
    {
        CE_Error("Failed to allocate asset cache data");
        return CE_ERROR;
    }

    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_ASSET_CACHE)
{
    // Clean up all assets in the cache
    cc_for_each(&component->m_assetData, key, el)
    {
        if (el->m_data)
        {
            CE_Engine_FreeAsset(el->m_data, el->m_type);
        }
    }
    cc_cleanup(&component->m_assetCount);
    cc_cleanup(&component->m_assetData);
    return CE_OK;
}

void *CE_Engine_CacheAsset(INOUT CE_ECS_Context* context, IN CE_TypeId assetType, IN const char *assetPath, IN_OPT const void *loadParams)
{
    CE_Engine_AssetCacheComponent *component = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_ASSET_CACHE);
    if (component == NULL)
    {
        CE_Error("Asset cache is not enabled");
        return NULL;
    }
    
    // Check if the asset is already cached
    CE_Engine_AssetCacheAsset *data = cc_get(&component->m_assetData, assetPath);
    if (data != NULL)
    {
        // Increment reference count
        uint32_t *count = cc_get(&component->m_assetCount, (uintptr_t)(data->m_data));
        if (count == NULL)
        {
            CE_Error("Inconsistent asset cache state for asset %s", assetPath);
            return NULL;
        }
        (*count)++;
        CE_Debug("Asset %s already cached, incremented ref count to %u", assetPath, *count);
        return data->m_data;
    }
    else
    {
        void *asset = CE_Engine_LoadAsset(assetType, assetPath, loadParams);
        if (asset == NULL)
        {
            CE_Error("Failed to load asset %s", assetPath);
            return NULL;
        }
        // Add the new asset to the cache
        bool inserted = false;
        CE_Engine_AssetCacheAsset newAsset = { .m_data = asset, .m_type = assetType };
        if (cc_insert(&component->m_assetData, assetPath, newAsset) != NULL)
        {
            if (cc_insert(&component->m_assetCount, (uintptr_t)asset, 1) != NULL)
            {
                inserted = true;
            }
        }
        
        if (!inserted)
        {
            CE_Error("Failed to insert asset %s into cache", assetPath);
            CE_Engine_FreeAsset(asset, assetType);
            return NULL;
        }

        CE_Debug("Cached new asset %s with initial ref count 1", assetPath);
        return asset;
    }
}

CE_Result CE_Engine_ReleaseAsset(INOUT CE_ECS_Context* context, IN void *asset)
{
    CE_Engine_AssetCacheComponent *component = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_ASSET_CACHE);
    if (component == NULL)
    {
        CE_Error("Asset cache is not enabled");
        return CE_ERROR;
    }

    uint32_t *count = cc_get(&component->m_assetCount, (uintptr_t)asset);
    if (count != NULL)
    {
        (*count)--;
        if (*count == 0)
        {
            bool found = false;
            // Find the asset in the data map to get its type and key
            cc_for_each(&component->m_assetData, key, el)
            {
                if (el->m_data == asset)
                {
                    // Free the asset
                    CE_Debug("Released asset from cache: %s", *key);
                    CE_Engine_FreeAsset(el->m_data, el->m_type);
                    // Remove from both maps
                    cc_erase_itr(&component->m_assetData, el);
                    cc_erase_itr(&component->m_assetCount, count);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                CE_Error("Inconsistent asset cache state while releasing asset");
                return CE_ERROR;
            }
        }
        return CE_OK;
    }
    else
    {
        CE_Error("Attempted to release an asset not in cache");
        return CE_ERROR;
    }
}

