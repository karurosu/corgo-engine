//
//  main.c
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Include main Engine  header
#include "engine/corgo.h"

// Playdate initialization is handled in backends/playdate.c
// The following is the stubbed version for other platforms, which simply initializes the engine and runs a basic loop.

#ifndef CE_BACKEND_PLAYDATE
int main(int argc, char* argv[])
{
	CE_ECS_Context *ecsContext;
	CE_ERROR_CODE errorCode;

	CE_DEBUG_UNUSED(argc);
	CE_DEBUG_UNUSED(argv);

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

	// Main loop
	bool isRunning = true;
	while (isRunning) {
		if (CE_Engine_Tick(ecsContext, &errorCode) != CE_OK) {
			CE_Error("Engine Tick failed with error code: %s", CE_GetErrorMessage(errorCode));
			isRunning = false;
		}
	}

	if (CE_Engine_Shutdown(ecsContext, &errorCode) != CE_OK) {
		CE_Error("Engine Shutdown failed with error code: %s", CE_GetErrorMessage(errorCode));
		return -1;
	}

	CE_free(ecsContext);
	ecsContext = NULL;

	return 0;
}
#endif // CE_BACKEND_PLAYDATE
