//
//  engine/input_actions.h
//  Main include for engine input actions.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_INPUT_ACTIONS_H
#define CORGO_ENGINE_INPUT_ACTIONS_H

#include "engine/config.h"
#include "engine/components/input.h"

// Include game input actions
#include "game/input_actions.h"
#ifndef CE_INPUT_ACTIONS_GAME
#define CE_INPUT_ACTIONS_GAME(CE_IA_DESC)
#endif // CE_INPUT_ACTIONS_GAME


// Input actions used by the demo
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES
#define CE_INPUT_ACTIONS_SAMPLE_SCENES(CE_IA_DESC) \
    CE_IA_DESC(DEMO_PRESS)\
    CE_IA_DESC(DEMO_RELEASE)\
    CE_IA_DESC(DEMO_HELD)\
    CE_IA_DESC(NEXT_INPUT_MAP)\
    CE_IA_DESC(PREVIOUS_INPUT_MAP)\
    CE_IA_DESC(DEMO_HELD_2)
#else
#define CE_INPUT_ACTIONS_SAMPLE_SCENES(CE_IA_DESC)
#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES


typedef enum CE_INPUT_ACTIONS {
#define CE_IA_DESC(name) CE_IA(name),
    CE_INPUT_ACTIONS_SAMPLE_SCENES(CE_IA_DESC)
    CE_INPUT_ACTIONS_GAME(CE_IA_DESC)
#undef CE_IA_DESC
    CE_ENGINE_INPUT_ACTIONS_COUNT // Invalid input action count
} CE_INPUT_ACTIONS;

/**
 * @brief Debug function to get the name of an input action as a string, used for debugging and logging purposes.
 * @param action[in] the input action enum value
 * @return a string literal with the name of the input action, or "Unknown" if the action is not recognized.
 */
const char* CE_Engine_GetInputActionNameDebugStr(IN CE_INPUT_ACTIONS action);

#endif // CORGO_ENGINE_INPUT_ACTIONS_H
