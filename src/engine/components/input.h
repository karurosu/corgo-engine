// 
//  engine/components/input.h
//  Input component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CE_ENGINE_COMPONENTS_INPUT_H
#define CE_ENGINE_COMPONENTS_INPUT_H

#include "ecs/types.h"
#include "engine/config.h"

typedef enum
{
	CE_Input_ButtonLeft   = (1<<0),
	CE_Input_ButtonRight	= (1<<1),
	CE_Input_ButtonUp		= (1<<2),
	CE_Input_ButtonDown	= (1<<3),
	CE_Input_ButtonB		= (1<<4),
	CE_Input_ButtonA		= (1<<5),
    CE_Input_Crank  = (1<<6) // 0 = docked, 1 = undocked
} CE_Input_RawInputMask;

typedef enum {
    CE_Input_Feature_Accelerometer = (1<<0),
    CE_Input_Feature_Crank = (1<<1)
} CE_Input_FeatureMask;

typedef  CE_Result (*CE_Input_ActionMapFunction)(IN CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);

typedef struct CE_InputComponent
{
    uint32_t m_rawInputsCurrent; // Current raw input state bitmask using CE_Input_RawInputMask
    uint32_t m_rawInputsPressed; // Raw inputs that were just pressed
    uint32_t m_rawInputsReleased; // Raw inputs that were just released
    uint32_t m_currentActionMap; // Points to the top of the stack of action maps
    uint32_t m_actionStates; // Current action state bitmask, gameplay code should read these
    uint32_t m_requestedFeatures; // Bitmask of requested input features using CE_Input_FeatureMask
    uint32_t m_enabledFeatures; // Bitmask of enabled input features using CE_Input_FeatureMask
    float m_crankAngle; // Current crank angle in degrees, 0-360, only updated if CE_Input_Feature_Crank is enabled
    float m_accelerometerX; // Current accelerometer X value, only updated if CE_Input_Feature_Accelerometer is enabled
    float m_accelerometerY; // Current accelerometer Y value, only updated if CE_Input_Feature_Accelerometer is enabled
    float m_accelerometerZ; // Current accelerometer Z value, only updated if CE_Input_Feature_Accelerometer is enabled
    CE_Input_ActionMapFunction m_actionMaps[CE_ENGINE_INPUT_MAP_STACK_SIZE]; // Stack of action map functions, these convert raw inputs to action states
} CE_InputComponent;

// Helpers
CE_Result CE_InputComponent_Reset(CE_InputComponent* inputComponent);
CE_Result CE_InputComponent_ResetAccelerometer(CE_InputComponent* inputComponent);

#endif // CE_ENGINE_COMPONENTS_INPUT_H