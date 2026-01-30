//
//  game/assets.h
//  Custom asset loaders for the game.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_GAME_ASSETS_H
#define CORGO_GAME_ASSETS_H


// Custom asset types go here.
// Don't forget the trailing slash.
// Follow the naming convention CE_ASSET_TYPE_<my name> for readability, i.e. CE_ASSET_TYPE_LEVEL_DATA
// The generated enum is not stable across builds, so do not serialize these values directly.
#define CE_ASSET_TYPES_GAME(X) \
/**
    X(CE_ASSET_TYPE_<my name>) \

**/

// Asset loaders for custom assets, one per type.
// May be defined per platform if needed, only one loader should be defined at a time or the build will fail.
// Don't forget the trailing slash.
//
// Parameters:
// <asset type> - Asset type enum value (must match one of the types defined above)
// <pointer type> - Pointer type returned by the loader
// <load parameters> - Struct type for load parameters, or CE_Asset_LoadParams_None if None
#define CE_ASSET_LOADERS_GAME(X) \
/**
    X(<asset type>, <pointer type>, <load parameters>) \

**/

#endif // CORGO_GAME_ASSETS_H