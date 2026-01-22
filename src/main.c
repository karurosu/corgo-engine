//
//  main.c
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdlib.h>

// Include Playdate API
#include "pd_api.h"

// Include memory management first to properly set up CC
#include "engine/core/memory.h"

// Include main ECS header
#include "ecs/ecs.h"

// Global ECS context
CE_ECS_Context *ecsContext;

// Tick helper
float lastTickTime = 0.0f;

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed
	
	if ( event == kEventInit )
	{
		// Initialize Engine
#ifdef CE_BACKEND_PLAYDATE
		pd->system->logToConsole("Received handler");
		CE_SetPlaydateAPI(pd);
		pd->system->logToConsole("Playdate API: %p", pd);
		pd->system->logToConsole("Playdate API cached: %p", CE_GetPlaydateAPI());
#endif
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if ( font == NULL )
			CE_Error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->resetElapsedTime();
		
		CE_Debug("Creating ECS Context");
		CE_ERROR_CODE errorCode;														
		ecsContext = CE_realloc(NULL, sizeof(CE_ECS_Context));
		CE_Debug("ECS Context allocated at %p with size %u bytes", ecsContext, sizeof(CE_ECS_Context));

		if (ecsContext == NULL) {
			CE_Error("Failed to allocate memory for ECS context");
			return -1;
		}

		if (CE_ECS_Init(ecsContext, &errorCode) != CE_OK) {
			CE_Error("ECS Initialization failed with error code: %s", CE_GetErrorMessage(errorCode));
			return -1;
		}
		
		CE_Debug("Engine Initialized in %f seconds", (double) pd->system->getElapsedTime());

		// Initialize timer
		pd->system->resetElapsedTime();
		lastTickTime = 0.0f;

		// Set update callback
		pd->system->setUpdateCallback(update, pd);
	}

	if (event == kEventTerminate)
	{
		CE_Debug("Shutting down Engine");
		// Cleanup ECS
		CE_ERROR_CODE errorCode;
		if (CE_ECS_Cleanup(ecsContext, &errorCode) != CE_OK) {
			CE_Error("ECS Cleanup failed with error code: %s", CE_GetErrorMessage(errorCode));
			return -1;
		}
	}
	
	return 0;
}


#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/2;
int y = (240-TEXT_HEIGHT)/2;

static int update(void* userdata)
{
	PlaydateAPI* pd = userdata;
	
	const float currentTime = pd->system->getElapsedTime();
	const float deltaTime = currentTime - lastTickTime;
	lastTickTime = currentTime;

	CE_Result result = CE_ECS_Tick(ecsContext, deltaTime, NULL);
	if (result != CE_OK) {
		CE_Error("ECS Tick failed with result code: %d", result);
		return -1;
	}

	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Hello Corgo!", strlen("Hello Corgo!"), kASCIIEncoding, x, y);
    
	pd->system->drawFPS(0,0);

	return 1;
}

