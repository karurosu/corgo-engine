//
//  engine/assets.h
//  List of asset loaders
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_ASSETS_H
#define CORGO_ENGINE_ASSETS_H

#include "ecs/types.h"
#include "core/platform.h"

// Include this here to expose API
#include "core/asset_cache.h"
#include "core/asset_loader.h"

#include "game/assets.h"
#ifndef CE_ASSET_TYPES_GAME
#define CE_ASSET_TYPES_GAME(X)
#endif

////////////////////////////////////
// Asset types are registered here
////////////////////////////////////

typedef enum CE_ASSET_TYPES {
    CE_ASSET_TYPE_INVALID,
    CE_ASSET_TYPE_BITMAP,
    CE_ASSET_TYPE_SOUND,
    CE_ASSET_TYPE_MUSIC,
    CE_ASSET_TYPE_FONT,
#define X(type) type,
    CE_ASSET_TYPES_GAME(X)
#undef X
    CE_ASSET_TYPE_COUNT
} CE_ASSET_TYPES;

////////////////////////////////////
// Asset parameters
////////////////////////////////////

typedef void CE_Asset_LoadParams_None; // Indicate the loader takes no parameters

////////////////////////////////////
// Asset loaders per platform
////////////////////////////////////

#ifdef CE_BACKEND_PLAYDATE
    // Playdate backend asset loaders
    #define CE_ASSET_LOADERS_ENGINE(X) \
        X(CE_ASSET_TYPE_BITMAP, LCDBitmap, CE_Asset_LoadParams_None) \
        X(CE_ASSET_TYPE_FONT, LCDFont, CE_Asset_LoadParams_None)
    
    #define CE_ASSET_LOADERS_STUBBED(X) \
        X(CE_ASSET_TYPE_SOUND) \
        X(CE_ASSET_TYPE_MUSIC) 

#else 
    // Stub asset loaders for unsupported backends
    #define CE_ASSET_LOADERS_STUBBED(X) \
        X(CE_ASSET_TYPE_BITMAP) \
        X(CE_ASSET_TYPE_SOUND) \
        X(CE_ASSET_TYPE_MUSIC) \
        X(CE_ASSET_TYPE_FONT)

#endif // CE_BACKEND_PLAYDATE

////////////////////////////////////
// Helpers
////////////////////////////////////

#ifndef CE_ASSET_LOADERS_ENGINE
#define CE_ASSET_LOADERS_ENGINE(X)
#endif

#ifndef CE_ASSET_LOADERS_GAME
#define CE_ASSET_LOADERS_GAME(X)
#endif

#define CE_ASSET_LOADERS(X) \
    CE_ASSET_LOADERS_ENGINE(X) \
    CE_ASSET_LOADERS_GAME(X)

////////////////////////////////////
// Auto generated declarations
////////////////////////////////////

// Defined assets
#define X(type, pointerType, loadParams) CE_DECLARE_ASSET_LOADER(type, pointerType, loadParams)
    CE_ASSET_LOADERS(X)
#undef X

// Stubbed assets
#define X(type) CE_DECLARE_ASSET_LOADER(type, void, void)
    CE_ASSET_LOADERS_STUBBED(X)
#undef X

#endif // CORGO_ENGINE_ASSETS_H