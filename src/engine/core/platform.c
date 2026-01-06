//
//  engine/core/platform.c
//  Platform-specific code.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "platform.h"

#ifdef CE_BACKEND_PLAYDATE
PlaydateAPI* CE_GetPlaydateAPI()
{
    return CE_pd;
}

void CE_SetPlaydateAPI(PlaydateAPI* pd)
{
    CE_pd = pd;
}

#endif