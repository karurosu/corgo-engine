//
//  engine/config.h
//  Engine specific configurations
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CONFIG_H
#define CORGO_ENGINE_CONFIG_H

// Asset cache initial size
#define CE_ENGINE_ASSET_CACHE_INITIAL_SIZE 16

// Display configurations
// Default refresh rate (0-50), may be changed in runtime by calling CE_Display_SetRefreshRate
#define CE_ENGINE_REFRESH_RATE_DEFAULT 30

// Default display scale (1,2,4,8), may be changed in runtime by calling CE_Display_SetScale
#define CE_ENGINE_SCALE_DEFAULT 1

// Include sample scenes, comment line below to exclude them from the build
#define CE_ENGINE_INCLUDE_SAMPLE_SCENES

// The default scene to load on startup, only if CE_ENGINE_SET_START_SCENE is not defined
#define CE_ENGINE_DEFAULT_SCENE SampleMenu

// Default input map size
#define CE_ENGINE_INPUT_MAP_STACK_SIZE 4

#endif // CORGO_ENGINE_CONFIG_H