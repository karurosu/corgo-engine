//
//  engine/core/init.c
//  Engine initialization functions.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"


CE_Result CE_Engine_Init(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode)
{
    CE_ResetElapsedTime();		

    CE_Debug("Welcome to Corgo Engine!");
    CE_Debug("Build Date: %s", CE_BUILD_DATETIME);
    
    CE_Debug("ECS Context allocated at %p with size %u bytes", context, sizeof(CE_ECS_Context));

    if (CE_ECS_Init(context, errorCode) != CE_OK) {
        CE_Error("ECS Initialization failed with error code: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    CE_Debug("Engine Initialized in %f seconds", (double) CE_GetElapsedTime());
    CE_ResetElapsedTime();

    CE_Debug("Scene Graph Initialized in %f seconds", (double) CE_GetElapsedTime());

    // Setup Screen
    CE_Display_SetRefreshRate(context, 60);

    // Load Scene
    CE_Result result = CE_ERROR;
#ifdef CE_ENGINE_SET_START_SCENE
    CE_Debug("Loading start scene: " CE_STRINGIFY(CE_ENGINE_SET_START_SCENE));
    result = CE_Scene_RequestLoad(context, CE_SCENE_LOAD_FUNCTION(CE_ENGINE_SET_START_SCENE), errorCode);
#else
    CE_Debug("Loading default scene: " CE_STRINGIFY(CE_ENGINE_DEFAULT_SCENE));
    result = CE_Scene_RequestLoad(context, CE_SCENE_LOAD_FUNCTION(CE_ENGINE_DEFAULT_SCENE), errorCode);
#endif
    if (result != CE_OK) {
        CE_Error("Failed to load start scene: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    // Enable debug if in a debug build
#ifdef CE_DEBUG_BUILD
    CE_Debug("Enabling debug mode");

    CE_Engine_SetGlobalDebug(context, true);

#if CE_SHOW_FPS
    CE_Engine_ShowFPSCounter(context, true);
#endif // CE_SHOW_FPS

#endif // CE_DEBUG_BUILD

    // Initialize timer
    CE_ResetElapsedTime();
    context->m_systemRuntimeData.m_lastTickTime = 0.0f;
    
    return CE_OK;
}

CE_Result CE_Engine_Shutdown(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode)
{
    return CE_ECS_Cleanup(context, errorCode);
}

CE_Result CE_Engine_Tick(INOUT CE_ECS_Context *context, OUT CE_ERROR_CODE *errorCode)
{
    const float currentTime = CE_GetElapsedTime();
	const float deltaTime = currentTime - context->m_systemRuntimeData.m_lastTickTime;
	context->m_systemRuntimeData.m_lastTickTime = currentTime;

	if (CE_ECS_Tick(context, deltaTime, errorCode) != CE_OK) {
		CE_Error("ECS Tick failed with result code: %d", CE_GetErrorMessage(*errorCode));
		return CE_ERROR;
	}

#if CE_ENGINE_ENABLE_ADAPTIVE_RENDERING
	if (CE_Engine_SceneGraph_IsDirty(context))
#endif
	{
		// Clear screen before rendering
		CE_Display_Clear(context);

		// Regenerate caches if needed
		if (CE_Engine_SceneGraph_UpdateRenderList(context, errorCode) != CE_OK) {
			CE_Error("Failed to update scene graph render list with error code: %s", CE_GetErrorMessage(*errorCode));
			return CE_ERROR;
		}

		// Render
		if (CE_ECS_TickRenderSystems(context, deltaTime, errorCode) != CE_OK) {
			CE_Error("ECS Tick Render Systems failed with result code: %d", CE_GetErrorMessage(*errorCode));
			return CE_ERROR;
		}
#if CE_ENGINE_ENABLE_ADAPTIVE_RENDERING
		CE_Engine_SceneGraph_ClearDirty(context);
#endif
	}

#ifdef CE_DEBUG_BUILD
	if (CE_ECS_TickDebugSystems(context, deltaTime, errorCode) != CE_OK) {
		CE_Error("ECS Tick Debug Systems failed with result code: %d", CE_GetErrorMessage(*errorCode));
		return CE_ERROR;
	}
#endif

	return CE_OK;
}