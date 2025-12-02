//
//  main.c
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

// Include Playdate API
#include "pd_api.h"

// Include main ECS header
#include "ecs/ecs.h"

// Global ECS context
CE_ECS_Context ecsContext;

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
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		pd->system->setUpdateCallback(update, pd);
		
		// Initialize ECS
		CE_ECS_Init(&ecsContext);
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
	
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Hello Corgo!", strlen("Hello Corgo!"), kASCIIEncoding, x, y);
    
	pd->system->drawFPS(0,0);

	return 1;
}

