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

#ifdef CE_BACKEND_PLAYDATE
    #define CE_Error(...) CE_GetPlaydateAPI()->system->error(__VA_ARGS__)
#else
    #define CE_Error(...) CE_Printf(__VA_ARGS__)
#endif

// Timer functions
/**
 * @brief Get the elapsed time in seconds since the last reset.
 * This is useful for measuring frame time and other time intervals.
 * 
 * @return Elapsed time in seconds.
 */
float CE_GetElapsedTime();

/**
 * @brief Reset the elapsed time counter to zero.
 */
void CE_ResetElapsedTime();


// Debug functions
#ifdef CE_DEBUG_BUILD

#ifdef CE_BACKEND_PLAYDATE
    #define CE_Debug(...) CE_GetPlaydateAPI()->system->logToConsole(__VA_ARGS__)
#else
    #define CE_Debug(...) CE_Printf(__VA_ARGS__)
#endif
  
/**
 * @brief Intentionally trigger a crash for testing crash logging and reporting.
 * This function executes an invalid instruction to cause a crash. It should only be used in testing
 * Literally this will cause a hard crash. Do not call this in production code or without a good reason.
 */
void CE_Debug_TriggerCrashForTesting(void);

#else
    #define CE_Debug(...)
    #define CE_Debug_TriggerCrashForTesting() ((void)0)
#endif // CE_DEBUG_BUILD


#endif // CORGO_ENGINE_CORE_PLATFORM_H