//
//  engine/asset_loaders/stubs.c
//  Stubbed asset loaders, to be used when no backend is available.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/assets.h"


// Generate stubbed implementations
#define X(type) CE_GENERATE_ASSET_LOADER_STUB(type)
    CE_ASSET_LOADERS_STUBBED(X)
#undef X
