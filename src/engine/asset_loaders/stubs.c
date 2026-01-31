//
//  engine/asset_loaders/stubs.c
//  Stubbed asset loaders, to be used when no backend is available.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/assets.h"

#ifndef CE_BACKEND_PLAYDATE
// Generate stubbed implementations
CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_SOUND)
CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_BITMAP)
CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_MUSIC)
CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_FONT)
#endif