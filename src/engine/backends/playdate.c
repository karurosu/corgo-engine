//
//  engine/backends/playdate.c
//  Playdate backend implementation for Corgo Engine.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifdef CE_BACKEND_PLAYDATE

#include "engine/corgo.h"
#include "engine/core/init.h" // Internal functions not exposed in the public API

// Include Playdate API
#include "pd_api.h"

// Global ECS context, managed by the PD backend for optimization
CE_ECS_Context *ecsContext;

static int update(void* userdata);


#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg;
	CE_ERROR_CODE errorCode;
	
	if ( event == kEventInit )
	{
		// Initialize Engine

		pd->system->logToConsole("Received PD Backend Handler");
		CE_SetPlaydateAPI(pd);
		pd->system->logToConsole("Playdate API: %p", pd);
		pd->system->logToConsole("Playdate API cached: %p", CE_GetPlaydateAPI());

        // Allocate ECS context
		ecsContext = CE_realloc(NULL, sizeof(CE_ECS_Context));

        if (ecsContext == NULL) {
			CE_Error("Failed to allocate memory for ECS context");
			return -1;
		}

        if (CE_Engine_Init(ecsContext, &errorCode) != CE_OK) {
			CE_Error("Engine Initialization failed with error code: %s", CE_GetErrorMessage(errorCode));
			return -1;
		}

		// Set update callback
		pd->system->setUpdateCallback(update, pd);
	}

	if (event == kEventTerminate)
	{
		if (CE_Engine_Shutdown(ecsContext, &errorCode) != CE_OK) {
            CE_Error("Engine Shutdown failed with error code: %s", CE_GetErrorMessage(errorCode));
            return -1;
        }

        CE_free(ecsContext);
        ecsContext = NULL;
	}
	
	return 0;
}

static int update(void* userdata)
{
	CE_ERROR_CODE errorCode;
	bool needsRedraw = false;
	if (CE_Engine_Tick(ecsContext, &needsRedraw, &errorCode) != CE_OK) {
        CE_Error("Engine Tick failed with error code: %s", CE_GetErrorMessage(errorCode));
        return -1;
    }

    return needsRedraw;
}

#endif // CE_BACKEND_PLAYDATE
