//
//  engine/core/asset_cache.h
//  A simple refcounted asset cache.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_ASSET_CACHE_H
#define CORGO_ENGINE_CORE_ASSET_CACHE_H

#include "ecs/types.h"
#include "engine/assets.h"

typedef struct CE_Engine_AssetCache_Asset {
    void *m_data;
    CE_TypeId m_type;
} CE_Engine_AssetCache_Asset;

typedef struct CE_Engine_AssetCache_Component {
    cc_map(uintptr_t, uint32_t) m_assetCount; // Cache the number of times an asset is referenced
    cc_map(const char *, CE_Engine_AssetCache_Asset) m_assetData; // Cache the asset's info
} CE_Engine_AssetCache_Component;

/** * @brief Cache an asset in the asset cache.
 * If the asset is already cached, increments the reference count and returns the existing asset.
 * If not, loads the asset, stores it in the cache, and returns it.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] assetType The type of the asset to cache.
 * @param[in] assetPath The path to the asset.
 * @param[in] loadParams Parameters required to load the asset.
 * @return Pointer to the cached asset, or NULL on failure.
 */
void *CE_Engine_CacheAsset(INOUT CE_ECS_Context* context, IN CE_TypeId assetType, IN const char *assetPath, IN_OPT const void* loadParams);

/**
 * @brief Release an asset from the asset cache.
 * Decrements the reference count, and if it reaches zero, unloads the asset.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] asset Pointer to the asset to release.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Engine_ReleaseAsset(INOUT CE_ECS_Context* context, IN void *asset); 

// Shortcuts to cache and release via macros per asset type

#define CE_DEFINE_ASSET_CACHE_LOAD_FUNCTION(type, pointer_type, load_params) \
    inline pointer_type *CE_Engine_CacheAsset_##type(INOUT CE_ECS_Context* context, IN const char* assetPath, IN const load_params* loadParams)\
    { \
        return (pointer_type *)CE_Engine_CacheAsset(context, type, assetPath, (const void *)loadParams); \
    }

#define CE_DEFINE_ASSET_CACHE_RELEASE_FUNCTION(type, pointer_type, load_params) \
    inline CE_Result CE_Engine_ReleaseAsset_##type(INOUT CE_ECS_Context* context, IN pointer_type *asset) \
    { \
        return CE_Engine_ReleaseAsset(context, (void *)asset); \
    }

#define CE_DECLARE_ASSET_CACHE_FUNCS(type, pointer_type, load_params) \
    CE_DEFINE_ASSET_CACHE_LOAD_FUNCTION(type, pointer_type, load_params) \
    CE_DEFINE_ASSET_CACHE_RELEASE_FUNCTION(type, pointer_type, load_params)

#define CE_CACHE_ASSET(context, type, path, loadParams) \
    CE_Engine_CacheAsset_##type(context, path, loadParams);

#define CE_CACHE_ASSET_TO_VAR(variable, context, type, path, loadParams) \
    CE_ASSET_PTR(type) variable = CE_Engine_CacheAsset_##type(context, path, loadParams);

#define CE_RELEASE_ASSET(context, type, asset) \
    CE_Engine_ReleaseAsset_##type(context, asset);

#define X(type, pointer_type, load_params) CE_DECLARE_ASSET_CACHE_FUNCS(type, pointer_type, load_params)
    CE_ASSET_LOADERS(X)
#undef X

#endif // CORGO_ENGINE_CORE_ASSET_CACHE_H