//
//  engine/core/debug.h
//  Engine debugging tools.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_DEBUG_H
#define CORGO_ENGINE_CORE_DEBUG_H

#ifdef CE_DEBUG_BUILD

#include "ecs/types.h"

/**
 * @brief Enable or disable the global debug component
 * @param context[in] The ECS context
 * @param enabled[in] Whether to enable or disable the global debug component
 * 
 * @return None
 */
void CE_Engine_SetGlobalDebug(IN CE_ECS_Context* context, IN bool enabled);

/**
 * @brief Enable or disable showing the FPS counter
 * It will enable the global debug component if it's not already enabled.
 * 
 * @param context[in] The ECS context
 * @param show[in] Whether to show or hide the FPS counter
 * @return None
 */
void CE_Engine_ShowFPSCounter(IN CE_ECS_Context* context, IN bool show);

#endif // CE_DEBUG_BUILD

#endif // CORGO_ENGINE_CORE_DEBUG_H