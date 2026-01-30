//
//  engine/core/asset_loader.h
//  Tools for dealing with asset loaders
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_ASSET_LOADER_H
#define CORGO_ENGINE_CORE_ASSET_LOADER_H

#include "ecs/types.h"

#define CE_DEFINE_ASSET_LOADER_LOAD(type) \
    type##_ptr_t CE_Engine_LoadAsset_##type(IN const char* assetPath, IN const type##_load_params_t* loadParams)

#define CE_DEFINE_ASSET_LOADER_FREE(type) \
    CE_Result CE_Engine_FreeAsset_##type(IN type##_ptr_t asset)

#define CE_DECLARE_ASSET_LOADER(type, pointer_type, load_params) \
    typedef pointer_type* type##_ptr_t; \
    typedef load_params type##_load_params_t; \
    CE_DEFINE_ASSET_LOADER_LOAD(type); \
    CE_DEFINE_ASSET_LOADER_FREE(type);

    
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

#endif // CORGO_ENGINE_CORE_ASSET_LOADER_H