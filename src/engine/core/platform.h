//
//  engine/core/platform.h
//  Platform-specific code.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_PLATFORM_H
#define CORGO_ENGINE_CORE_PLATFORM_H

#include <stdbool.h>

// PD API caching and access
#ifdef CE_BACKEND_PLAYDATE

#include "pd_api.h"
PlaydateAPI* CE_GetPlaydateAPI();
void CE_SetPlaydateAPI(PlaydateAPI* pd);

#endif // CE_BACKEND_PLAYDATE

// Logging functions

// Utility function to pretty print to console
void CE_Printf(const char* format, ...);
void CE_SetLoggingEnabled(bool enabled);

#ifdef CE_DEBUG_BUILD
    #ifdef CE_BACKEND_PLAYDATE
        #define CE_Debug(...) CE_GetPlaydateAPI()->system->logToConsole(__VA_ARGS__);
    #else
        #define CE_Debug(...) CE_Printf(__VA_ARGS__);
    #endif
#else
    #define CE_Debug(...)
#endif // CE_DEBUG_BUILD

#ifdef CE_BACKEND_PLAYDATE
    #define CE_Error(...) CE_GetPlaydateAPI()->system->error(__VA_ARGS__);
#else
    #define CE_Error(...) CE_Printf(__VA_ARGS__);
#endif


#endif // CORGO_ENGINE_CORE_PLATFORM_H