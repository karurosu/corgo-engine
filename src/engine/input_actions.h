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
#define CE_INPUT_ACTIONS_GAME(X)
#endif // CE_INPUT_ACTIONS_GAME


// Input actions used by the demo
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES
#define CE_INPUT_ACTIONS_SAMPLE_SCENES(X) \
    X(CE_IA_DEMO_PRESS)\
    X(CE_IA_DEMO_RELEASE)\
    X(CE_IA_DEMO_HELD)\
    X(CE_IA_NEXT_INPUT_MAP)\
    X(CE_IA_PREVIOUS_INPUT_MAP)\
    X(CE_IA_DEMO_HELD_2)
#else
#define CE_INPUT_ACTIONS_SAMPLE_SCENES(X)
#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES


typedef enum CE_INPUT_ACTIONS {
#define X(name) name,
    CE_INPUT_ACTIONS_SAMPLE_SCENES(X)
    CE_INPUT_ACTIONS_GAME(X)
#undef X
    CE_ENGINE_INPUT_ACTIONS_COUNT // Invalid input action count
} CE_INPUT_ACTIONS;

/**
 * @brief Debug function to get the name of an input action as a string, used for debugging and logging purposes.
 * @param action[in] the input action enum value
 * @return a string literal with the name of the input action, or "Unknown" if the action is not recognized.
 */
const char* CE_Engine_GetInputActionNameDebugStr(IN CE_INPUT_ACTIONS action);

#endif // CORGO_ENGINE_INPUT_ACTIONS_H
