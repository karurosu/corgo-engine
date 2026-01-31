//
//  engine/core/asset_loader.h
//  Tools for dealing with asset loaders
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/assets.h"

void *CE_Engine_LoadAsset(IN CE_TypeId assetType, IN const char* assetPath, IN const void* loadParams)
{
    switch (assetType)
    {
        #define X(type, ...) \
        case type: \
            return (void *)CE_Engine_LoadAsset_##type(assetPath, (const type##_load_params_t *)loadParams);
        CE_ASSET_LOADERS(X)
        #undef X
        default:
            CE_Error("Unknown asset type %d", assetType);
            return NULL;
    }
}

CE_Result CE_Engine_FreeAsset(IN void *asset, IN CE_TypeId assetType)
{
    switch (assetType)
    {
        #define X(type, ...) \
        case type: \
            return CE_Engine_FreeAsset_##type((CE_ASSET_PTR(type))asset);
        CE_ASSET_LOADERS(X)
        #undef X
        default:
            CE_Error("Unknown asset type %d", assetType);
            return CE_ERROR;
    }
}
