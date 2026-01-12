//
//  engine/core/platform.c
//  Platform-specific code.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "platform.h"

#include <stdarg.h>
#include <stdio.h>

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

void CE_Printf(const char* format, ...) {
    if (CE_Logging_enabled) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        printf("\n");
        va_end(args);
    }
}

void CE_SetLoggingEnabled(bool enabled)
{
    CE_Logging_enabled = enabled;
}
