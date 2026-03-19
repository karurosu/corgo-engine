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

typedef struct CE_InputComponent CE_InputComponent; // Forward declaration to avoid circular dependency with input map functions

typedef void (*CE_Input_ActionMapFunction)(IN CE_ECS_Context* context, IN CE_InputComponent* inputComponent, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode);

typedef struct CE_InputComponent
{
    uint32_t m_rawInputsCurrent; // Current raw input state bitmask using CE_Input_RawInputMask
    uint32_t m_rawInputsPressed; // Raw inputs that were just pressed
    uint32_t m_rawInputsReleased; // Raw inputs that were just released
    int32_t m_currentActionMap; // Points to the top of the stack of action maps
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

// Public API

/**
 * @brief Macro to declare an input action map function with the correct signature.
 * @param name[in] unique name for the action map function
 */
#define CE_DECLARE_ACTION_MAP_FUNCTION(name) void name(INOUT struct CE_ECS_Context* context, IN CE_InputComponent* inputComponent, IN float deltaTime, OUT_OPT CE_ERROR_CODE* errorCode)

/**
 * @brief Pushes a new action map function onto the stack and makes it the current action map.
 * It may fail if the stack is full.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param actionMapFunction[in] The action map function to push of type CE_Input_ActionMapFunction
 * @param errorCode[out] Optional pointer to receive an error code if the push fails.
 * @return CE_OK if the action map was successfully pushed, CE_ERROR if it failed.
 */
CE_Result CE_Input_PushActionMap(INOUT struct CE_ECS_Context* context, CE_Input_ActionMapFunction actionMapFunction, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Pops the current action map function from the stack.
 * It may fail if the stack is empty.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param errorCode[out] Optional pointer to receive an error code if the pop fails.
 * @return CE_OK if the action map was successfully popped, CE_ERROR if it failed.
 */
CE_Result CE_Input_PopActionMap(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);


/**
 * @brief Clears all action map functions from the stack.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param errorCode[out] Optional pointer to receive an error code if the clear operation fails.
 * @return CE_OK if the action maps were successfully cleared, CE_ERROR if it failed.
 */
CE_Result CE_Input_ClearActionMaps(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);


/**
 * @brief Enables or disables a specific input feature.
 * It may take 1-2 frames for the change to take effect.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param featureMask[in] The feature mask to enable or disable.
 * @param enabled[in] True to enable the feature, false to disable it.
 * @param errorCode[out] Optional pointer to receive an error code if the operation fails.
 * @return CE_OK if the feature was successfully set, CE_ERROR if it failed.
 */

CE_Result CE_Input_SetFeature(INOUT struct CE_ECS_Context* context, CE_Input_FeatureMask featureMask, bool enabled, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Checks if a specific input feature is currently enabled.
 * Features waiting to be enabled will return false, so this can be used to wait for a feature to be enabled before trying to use it.
 * 
 * @param context[in,out] The ECS context to operate on.
 * @param featureMask[in] The feature mask to check.
 * @return true if the feature is enabled, false otherwise.
 */
bool CE_Input_IsFeatureEnabled(INOUT struct CE_ECS_Context* context, CE_Input_FeatureMask featureMask);

/**
 * @brief Helper macro to get the bitmask for an input action, given its enum value.
 * @param action[in] the input action enum value
 * @return a bitmask with the bit corresponding to the input action set, used to set/check the action state in CE_InputComponent.m_actionStates
 */
#define CE_IA_MASK(action) (1 << (action))

#endif // CE_ENGINE_COMPONENTS_INPUT_H