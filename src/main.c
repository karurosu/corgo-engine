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

// Global ECS context
CE_ECS_Context *ecsContext;

// Tick helper
float lastTickTime = 0.0f;

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const char* font2path = "/System/Fonts/Roobert-10-Bold.pft";
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

		// Initialize scene graph
		result = CE_Engine_SceneGraph_Init(ecsContext, &errorCode);
		if (result != CE_OK) {
			CE_Error("Failed to initialize scene graph: %s", CE_GetErrorMessage(errorCode));
			return -1;
		}

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
			result = CE_TextLabelComponent_setText(ecsContext, textLabelComponent, "Hello, Corgo Engine!");
			if (result != CE_OK) {
				CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}
			result = CE_TextLabelComponent_setFont(ecsContext, textLabelComponent, fontpath);
			if (result != CE_OK) {
				CE_Error("Failed to set font for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}
			result = CE_TextLabelComponent_setFont(ecsContext, textLabelComponent, font2path);
			if (result != CE_OK) {
				CE_Error("Failed to set font for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Get bounds of the text
			result = CE_TextLabelComponent_getTextBounds(ecsContext, textLabelComponent, &x_size, &y_size);
			if (result != CE_OK) {
				CE_Error("Failed to get text bounds for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Add to graph
			CE_SceneGraph_AddChild(ecsContext, CE_SceneGraph_GetSceneRootId(ecsContext), entityId, false, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add demo entity to scene graph: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			CE_Debug("Text bounds: %d x %d", x_size, y_size);
			CE_TransformComponent_setPosition(ecsContext, transformComponent, (CE_GetDisplayWidth()-x_size)/2, (CE_GetDisplayHeight()-y_size)/2, 0);

			// Create a second entity to test the scene graph Z ordering
			CE_Id entityId2 = CE_INVALID_ID;
			CE_Id transformComponentId2 = CE_INVALID_ID;
			CE_Id textLabelComponentId2 = CE_INVALID_ID;
			CE_TransformComponent* transformComponent2 = NULL;
			CE_TextLabelComponent* textLabelComponent2 = NULL;
			int x_size2 = 0;
			int y_size2 = 0;

			result = CE_ECS_CreateEntity(ecsContext, &entityId2, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to create demo entity 2: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			result = CE_Entity_AddComponent(ecsContext, entityId2, CE_TRANSFORM_COMPONENT, &transformComponentId2, (void**)&transformComponent2, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add TransformComponent to demo entity 2: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			result = CE_Entity_AddComponent(ecsContext, entityId2, CE_TEXT_LABEL_COMPONENT, &textLabelComponentId2, (void**)&textLabelComponent2, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add TextLabelComponent to demo entity 2: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Set text and font
			result = CE_TextLabelComponent_setText(ecsContext, textLabelComponent2, "WOOF!!");
			if (result != CE_OK) {
				CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			result = CE_TextLabelComponent_setFont(ecsContext, textLabelComponent2, fontpath);
			if (result != CE_OK) {
				CE_Error("Failed to set font for TextLabelComponent 2: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}
			
			textLabelComponent2->m_inverted = true; // Draw in white

			// Get bounds of the text
			result = CE_TextLabelComponent_getTextBounds(ecsContext, textLabelComponent2, &x_size2, &y_size2);
			if (result != CE_OK) {
				CE_Error("Failed to get text bounds for TextLabelComponent 2: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			// Add to graph
			CE_SceneGraph_AddChild(ecsContext, CE_SceneGraph_GetSceneRootId(ecsContext), entityId2, false, &errorCode);
			if (result != CE_OK) {
				CE_Error("Failed to add demo entity 2 to scene graph: %s", CE_GetErrorMessage(errorCode));
				return -1;
			}

			CE_Debug("Text bounds: %d x %d", x_size2, y_size2);
			CE_TransformComponent_setPosition(ecsContext, transformComponent2, (CE_GetDisplayWidth()-x_size2)/2, (CE_GetDisplayHeight()-y_size2)/2, 0);
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
		CE_TransformComponent_setPosition(ecsContext, transformComponent, x, y, 0);
	}

#ifdef CE_BACKEND_PLAYDATE
	pd->system->drawFPS(0,0);
#endif

	return 1;
}

