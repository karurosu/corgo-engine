//
//  engine/core/asset_loader.h
//  Tools for dealing with asset loaders
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_ASSET_LOADER_H
#define CORGO_ENGINE_CORE_ASSET_LOADER_H

#include "ecs/types.h"
#include "engine/assets.h"
    
// Shortcuts for uncached loading/releasing
#define CE_ASSET_PTR(type) type##_ptr_t

/**
 * @brief Load an asset of the given type and return the pointer.
 * The type of the returned pointer is CE_ASSET_PTR(type)
 * This function like macro does not use the asset cache. Multiple calls will load multiple copies of the asset.
 * 
 * @param[in] type Asset type.
 * @param[in] path Path to the asset.
 * @param[out] variable Variable to store the loaded asset pointer.
 */
#define CE_LOAD_ASSET(type, path, loadParams) \
    CE_Engine_LoadAsset_##type(path, loadParams);

/**
 * @brief Load an asset of the given type and save it to a variable.
 * Equivalent to CE_ASSET_PTR(type) variable = CE_LOAD_ASSET(type, path, loadParams)
 * This function like macro does not use the asset cache. Multiple calls will load multiple copies of the asset.
 * 
 * @param[in] type Asset type.
 * @param[in] path Path to the asset.
 * @param[out] variable Variable to store the loaded asset pointer.
 */
#define CE_LOAD_ASSET_TO_VAR(type, path, variable, loadParams) \
    CE_ASSET_PTR(type) variable = CE_LOAD_ASSET(type, path, loadParams);

/**
 * @brief Free an asset of the given type.
 * This function like macro does not use the asset cache, it will be unloaded immediately.
 * 
 * @param[in] type Asset type.
 * @param[in] asset Pointer to the asset to free.
 */
#define CE_FREE_ASSET(type, asset) \
    CE_Engine_FreeAsset_##type(asset);

/**
 * @brief Generate stub implementations for asset loaders.
 * These implementations will log an error and return NULL or CE_ERROR.
 */
#define CE_GENERATE_ASSET_LOADER_STUB(type) \
    CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(type) \
    { \
        CE_Error("Asset loader stub called for asset: %s", assetPath ? assetPath : "NULL"); \
        return NULL; \
    } \
    CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(type) \
    { \
        CE_Error("Asset free stub called"); \
        return CE_ERROR; \
    }

/**
 * @brief Load an asset without caching.
 * This is a generic function that calls the appropriate load function based on the asset type.
 * It is slower than the macros but it can work for runtime type information.
 * 
 * @param[in] assetPath Path to the asset to load.
 * @param[in] loadParams Parameters required to load the asset.
 * @return Pointer to the loaded asset, or NULL on failure.
 */
void *CE_Engine_LoadAsset(IN CE_TypeId assetType, IN const char* assetPath, IN const void* loadParams);

/**
 * @brief Free an asset that was loaded without caching.
 * This is a generic function that calls the appropriate free function based on the asset type.
 * It is slower than the macros but it can work for runtime type information.
 * 
 * @param[in] asset Pointer to the asset to free.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Engine_FreeAsset(IN void *asset, IN CE_TypeId assetType);


// Auto declared asset loader functions
#define CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(type) \
    type##_ptr_t CE_Engine_LoadAsset_##type(IN const char* assetPath, IN const type##_load_params_t* loadParams)

#define CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(type) \
    CE_Result CE_Engine_FreeAsset_##type(IN type##_ptr_t asset)

#define CE_DECLARE_ASSET_LOADER(type, pointer_type, load_params) \
    typedef pointer_type *type##_ptr_t; \
    typedef load_params type##_load_params_t; \
    CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(type); \
    CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(type);

#define X(type, pointerType, loadParams) CE_DECLARE_ASSET_LOADER(type, pointerType, loadParams)
    CE_ASSET_LOADERS(X)
#undef X

#endif // CORGO_ENGINE_CORE_ASSET_LOADER_H