//
//  engine/components/input.c
//  Input component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_INPUT_COMPONENT)
{
    CE_InputComponent_Reset(component);
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_INPUT_COMPONENT)
{
    return CE_OK;
}

CE_Result CE_InputComponent_Reset(CE_InputComponent* inputComponent)
{
    inputComponent->m_rawInputsCurrent = 0;
    inputComponent->m_rawInputsPressed = 0;
    inputComponent->m_rawInputsReleased = 0;
    inputComponent->m_currentActionMap = -1;
    inputComponent->m_actionStates = 0;
    inputComponent->m_enabledFeatures = 0;
    inputComponent->m_crankAngle = 0.0f;
    inputComponent->m_accelerometerX = 0.0f;
    inputComponent->m_accelerometerY = 0.0f;
    inputComponent->m_accelerometerZ = 0.0f;
    for (int i = 0; i < CE_ENGINE_INPUT_MAP_STACK_SIZE; i++) {
        inputComponent->m_actionMaps[i] = NULL;
    }
    return CE_OK;
}

CE_Result CE_InputComponent_ResetAccelerometer(CE_InputComponent* inputComponent)
{
    inputComponent->m_accelerometerX = 0.0f;
    inputComponent->m_accelerometerY = 0.0f;
    inputComponent->m_accelerometerZ = 0.0f;
    return CE_OK;
}

CE_Result CE_Input_PushActionMap(INOUT struct CE_ECS_Context* context, CE_Input_ActionMapFunction actionMapFunction, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    if (inputComponent->m_currentActionMap >= CE_ENGINE_INPUT_MAP_STACK_SIZE - 1) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_INPUT_TOO_MANY_ACTION_MAPS);
        return CE_ERROR;
    }
    inputComponent->m_currentActionMap++;
    inputComponent->m_actionMaps[inputComponent->m_currentActionMap] = actionMapFunction;
    return CE_OK;
}

CE_Result CE_Input_PopActionMap(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    if (inputComponent->m_currentActionMap < 0) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_ENGINE_INPUT_NO_ACTION_MAPS);
        return CE_ERROR;
    }
    inputComponent->m_actionMaps[inputComponent->m_currentActionMap] = NULL;
    inputComponent->m_currentActionMap--;
    return CE_OK;
}

CE_Result CE_Input_ClearActionMaps(INOUT struct CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    for (int i = 0; i <= inputComponent->m_currentActionMap; i++) {
        inputComponent->m_actionMaps[i] = NULL;
    }
    inputComponent->m_currentActionMap = -1;
    return CE_OK;
}

CE_Result CE_Input_SetFeature(INOUT struct CE_ECS_Context* context, CE_Input_FeatureMask featureMask, bool enabled, OUT_OPT CE_ERROR_CODE* errorCode)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    if (enabled) {
        inputComponent->m_requestedFeatures |= featureMask;
    } else {
        inputComponent->m_requestedFeatures &= ~featureMask;
    }
    return CE_OK;
}

bool CE_Input_IsFeatureEnabled(INOUT struct CE_ECS_Context* context, CE_Input_FeatureMask featureMask)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    return (inputComponent->m_enabledFeatures & featureMask) != 0;
}

const char* CE_Engine_GetInputActionNameDebugStr(IN CE_INPUT_ACTIONS action)
{
#ifdef CE_DEBUG_BUILD
    switch (action) {
#define CE_IA_DESC(name) case CE_IA(name): return CE_STRINGIFY(CE_IA(name));
        CE_INPUT_ACTIONS_SAMPLE_SCENES(CE_IA_DESC)
        CE_INPUT_ACTIONS_GAME(CE_IA_DESC)
#undef CE_IA_DESC
        default: return "Unknown";
    }
#else
    return "NotInDebugBuild";
#endif
}
