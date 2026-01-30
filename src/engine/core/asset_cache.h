//
//  engine/core/asset_cache.h
//  A simple refcounted asset cache.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_ASSET_CACHE_H
#define CORGO_ENGINE_CORE_ASSET_CACHE_H

#include "ecs/types.h"

typedef struct CE_Engine_AssetCache_Asset {
    void *m_asset;
    uint8_t m_refcount;
    CE_TypeId m_assetType;
} CE_Engine_AssetCache_Asset;

typedef struct CE_Engine_AssetCache_Component {
    bool m_enabled;
    cc_map(uintptr_t, CE_Engine_AssetCache_Asset) m_assets;
} CE_Engine_AssetCache_Component;

#define CE_DECLARE_ASSET_CACHE_FUNCS(type, pointer_type, load_params) \
    type##_ptr_t CE_Engine_CacheAsset_##type(IN const char* assetPath, IN const type##_load_params_t* loadParams); \
    CE_Result CE_Engine_ReleaseAsset_##type(IN type##_ptr_t asset);


void *CE_Engine_CacheAsset(INOUT CE_ECS_Context* context, IN CE_TypeId assetType, IN const char *assetPath, void *loadParams);

/**
 * @brief Release an asset from the asset cache.
 * Decrements the reference count, and if it reaches zero, unloads the asset.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] asset Pointer to the asset to release.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Engine_ReleaseAsset(INOUT CE_ECS_Context* context, IN void *asset); 

#endif // CORGO_ENGINE_CORE_ASSET_CACHE_H