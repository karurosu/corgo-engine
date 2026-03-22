//
//  engine/core/platform.c
//  Platform-specific code.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "platform.h"

#include <stdarg.h>

#ifndef CE_ARM_BUILD
#include <stdio.h>
#endif

static bool CE_Logging_enabled = true;

#ifdef CE_BACKEND_PLAYDATE
static PlaydateAPI* CE_pd = NULL;

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
#ifndef CE_ARM_BUILD
    if (CE_Logging_enabled) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        printf("\n");
        va_end(args);
    }
#endif
}

void CE_SetLoggingEnabled(bool enabled)
{
    CE_Logging_enabled = enabled;
}

#ifdef CE_ARM_BUILD
// printf needs a putchar_ in order to work with armgcc
int putchar_(int c)
{
    CE_GetPlaydateAPI()->system->logToConsole("%c", (char)c);
    return c;
}
#endif

#ifdef CE_DEBUG_BUILD
void CE_Debug_TriggerCrashForTesting(void)
{
	/*
	 * Intentionally execute from an invalid system-space address.
	 * This is only used to generate a reproducible crashlog sample.
	 */
#ifdef CE_ARM_BUILD
    CE_Debug("Going to crash now, night night!.");
	volatile int (*bad_instruction)(void) = (int (*)(void))0xE0000000;
	bad_instruction();
#endif
}
#endif // CE_DEBUG_BUILD
