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
    inputComponent->m_currentActionMap = 0;
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