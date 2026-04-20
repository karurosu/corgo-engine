//
//  engine/assets.h
//  List of asset loaders
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_ASSETS_H
#define CORGO_ENGINE_ASSETS_H

#include "ecs/types.h"
#include "core/platform.h"

#include "game/assets.h"
#ifndef CE_ASSET_TYPES_GAME
#define CE_ASSET_TYPES_GAME(CE_ASSET_TYPE)
#endif

////////////////////////////////////
// Asset types are registered here
////////////////////////////////////

typedef enum CE_ASSET_TYPES {
    CE_ASSET_TYPE_INVALID,
    CE_ASSET_TYPE_BITMAP,
    CE_ASSET_TYPE_BITMAP_TABLE,
    CE_ASSET_TYPE_SOUND,
    CE_ASSET_TYPE_MUSIC,
    CE_ASSET_TYPE_FONT,
#define CE_ASSET_TYPE(type) type,
    CE_ASSET_TYPES_GAME(CE_ASSET_TYPE)
#undef CE_ASSET_TYPE
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
    #define CE_ASSET_LOADERS_ENGINE(CE_ASSET_LOADER) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_BITMAP, LCDBitmap, CE_Asset_LoadParams_None) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_BITMAP_TABLE, LCDBitmapTable, CE_Asset_LoadParams_None) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_FONT, LCDFont, CE_Asset_LoadParams_None) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_SOUND, void, void) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_MUSIC, void, void) \

#else 
    // Default loaders (stubs)
    #define CE_ASSET_LOADERS_ENGINE(CE_ASSET_LOADER) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_BITMAP, void, void) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_BITMAP_TABLE, void, void) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_SOUND, void, void) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_MUSIC, void, void) \
        CE_ASSET_LOADER(CE_ASSET_TYPE_FONT, void, void)

#endif // CE_BACKEND_PLAYDATE

////////////////////////////////////
// Helpers
////////////////////////////////////

#ifndef CE_ASSET_LOADERS_ENGINE
#define CE_ASSET_LOADERS_ENGINE(CE_ASSET_LOADER)
#endif

#ifndef CE_ASSET_LOADERS_GAME
#define CE_ASSET_LOADERS_GAME(CE_ASSET_LOADER)
#endif

#define CE_ASSET_LOADERS(CE_ASSET_LOADER) \
    CE_ASSET_LOADERS_ENGINE(CE_ASSET_LOADER) \
    CE_ASSET_LOADERS_GAME(CE_ASSET_LOADER)

////////////////////////////////////
// Auto generated declarations
////////////////////////////////////

#include "core/asset_loader.h"
// Include this here to expose API
#include "core/asset_cache.h"


#endif // CORGO_ENGINE_ASSETS_H