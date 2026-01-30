//
//  engine/core/asset_cache.c
//  A simple refcounted asset cache.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "asset_cache.h"
#include "ecs/ecs.h"
#include "platform.h"
#include "memory.h"
#include "engine/config.h"

// Component functions
CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_ASSET_CACHE)
{
    component->m_enabled = false;
    cc_init(&component->m_assets);
    if (!cc_reserve(&component->m_assets, CE_ENGINE_ASSET_CACHE_INITIAL_SIZE))
    {
        CE_Error("Failed to allocate asset cache");
        return CE_ERROR;
    }
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_ASSET_CACHE)
{
    // Clean up all assets in the cache
    cc_for_each(&component->m_assets, key, el)
    {
        if (el->m_asset)
        {
            // Release asset
        }
    }
    cc_cleanup(&component->m_assets);
    return CE_OK;
}
