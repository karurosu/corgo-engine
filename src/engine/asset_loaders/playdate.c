//
//  engine/asset_loaders/playdate.c
//  Playdate specific asset loaders
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/assets.h"
#include "engine/core/platform.h"
#include "engine/core/memory.h"

#ifdef CE_BACKEND_PLAYDATE

// Bitmap asset loader
CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(CE_ASSET_TYPE_BITMAP)
{
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    if (!pd || !assetPath)
    {
        CE_Error("Invalid parameters to load bitmap asset");
        return NULL;
    }

    const char *err;
    LCDBitmap* bitmap = pd->graphics->loadBitmap(assetPath, &err);
    if (!bitmap)
    {
        CE_Error("Failed to load bitmap asset: %s. Error: %s", assetPath, err ? err : "unknown");
        return NULL;
    }

    return bitmap;
}

CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(CE_ASSET_TYPE_BITMAP)
{
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    if (!pd || !asset)
    {
        CE_Error("Invalid parameters to free bitmap asset");
        return CE_ERROR;
    }

    pd->graphics->freeBitmap(asset);
    return CE_OK;
}

// Font asset loader
CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(CE_ASSET_TYPE_FONT)
{
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    if (!pd || !assetPath)
    {
        CE_Error("Invalid parameters to load font asset");
        return NULL;
    }

    const char *err;
    LCDFont* font = pd->graphics->loadFont(assetPath, &err);
    if (!font)
    {
        CE_Error("Failed to load font asset: %s. Error: %s", assetPath, err ? err : "unknown");
        return NULL;
    }

    return font;
}

CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(CE_ASSET_TYPE_FONT)
{
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    if (!pd || !asset)
    {
        CE_Error("Invalid parameters to free font asset");
        return CE_ERROR;
    }

    CE_free(asset);
    return CE_OK;
}

CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_SOUND)
CE_GENERATE_ASSET_LOADER_STUB(CE_ASSET_TYPE_MUSIC)

#endif // CE_BACKEND_PLAYDATE