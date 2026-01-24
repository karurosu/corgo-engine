//
//  main.c
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdlib.h>

// Include Playdate API
#include "pd_api.h"

// Include memory management first to properly set up CC
#include "engine/core/memory.h"
#include "engine/core/display.h"

// Include main ECS header
#include "ecs/ecs.h"

// Helper include for demo
#include "engine/components/helpers.h"

// Global ECS context
CE_ECS_Context *ecsContext;

// Tick helper
float lastTickTime = 0.0f;

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

// Demo variables, global to keep it simple
int x_size = 0;
int y_size = 0;
int x = 0;
int y = 0;
int x_speed = 1;
int y_speed = 1;
CE_Id transformComponentId = CE_INVALID_ID;
CE_TransformComponent* transformComponent = NULL;
CE_Id textLabelComponentId = CE_INVALID_ID;
CE_TextLabelComponent* textLabelComponent = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed
	CE_Result result;
	
	if ( event == kEventInit )
	{
		// Initialize Engine
#ifdef CE_BACKEND_PLAYDATE
		pd->system->logToConsole("Received handler");
		CE_SetPlaydateAPI(pd);
		pd->system->logToConsole("Playdate API: %p", pd);
		pd->system->logToConsole("Playdate API cached: %p", CE_GetPlaydateAPI());
		pd->system->resetElapsedTime();

		// Set screen properties
		pd->display->setScale(1);
#endif
				
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

		// Demo code to print a text label using ECS
		// TODO: move this once scene management is implemented
		{
			CE_Id entityId = CE_INVALID_ID;
			result = CE_ECS_CreateEntity(ecsContext, &entityId, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to create demo entity: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			result = CE_Entity_AddComponent(ecsContext, entityId, CE_TRANSFORM_COMPONENT, &transformComponentId, (void**)&transformComponent, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add TransformComponent to demo entity: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			result = CE_Entity_AddComponent(ecsContext, entityId, CE_TEXT_LABEL_COMPONENT, &textLabelComponentId, (void**)&textLabelComponent, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add TextLabelComponent to demo entity: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Set text and font
			result = CE_TextLabelComponent_setText(textLabelComponent, "Hello, Corgo Engine!");
			if (result != CE_OK) {
				CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}
			result = CE_TextLabelComponent_setFont(textLabelComponent, fontpath);
			if (result != CE_OK) {
				CE_Error("Failed to set font for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Get bounds of the text
			result = CE_TextLabelComponent_getTextBounds(textLabelComponent, &x_size, &y_size);
			if (result != CE_OK) {
				CE_Error("Failed to get text bounds for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			CE_Debug("Text bounds: %d x %d", x_size, y_size);
			CE_TransformComponent_setPosition(transformComponent, (CE_GetDisplayWidth()-x_size)/2, (CE_GetDisplayHeight()-y_size)/2, 0);
		}

#ifdef CE_BACKEND_PLAYDATE
		// Initialize timer
		pd->system->resetElapsedTime();
		lastTickTime = 0.0f;

		// Set update callback
		pd->system->setUpdateCallback(update, pd);
#endif
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

static int update(void* userdata)
{
	CE_Result result;
	float currentTime = 0.0f;
	float deltaTime = 0.0f;
	CE_ERROR_CODE errorCode;

#ifdef CE_BACKEND_PLAYDATE
	PlaydateAPI* pd = userdata;
	
	currentTime = pd->system->getElapsedTime();
	deltaTime = currentTime - lastTickTime;
	lastTickTime = currentTime;

	// Clear screen
	pd->graphics->clear(kColorWhite);
#endif

	result = CE_ECS_Tick(ecsContext, deltaTime, &errorCode);
	if (result != CE_OK) {
		CE_Error("ECS Tick failed with result code: %d", CE_GetErrorMessage(errorCode));
		return -1;
	}

	// Simple animation for demo
	{
		x += x_speed;
		y += y_speed;
		if (x <= 0 || x + x_size >= CE_GetDisplayWidth()) {
			x_speed = -x_speed;
		}
		if (y <= 0 || y + y_size >= CE_GetDisplayHeight()) {
			y_speed = -y_speed;
		}

		// Update transform component position
		CE_TransformComponent_setPosition(transformComponent, x, y, 0);
	}

#ifdef CE_BACKEND_PLAYDATE
	pd->system->drawFPS(0,0);
#endif

	return 1;
}

