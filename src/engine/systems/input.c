//
//  engine/systems/input.c
//  System that deals with high level input handling.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_GLOBAL_INPUT_SYSTEM)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    
    // Clear action states, these will be set by the action map functions
    inputComponent->m_actionStates = 0;

    if (inputComponent->m_currentActionMap == -1) {
        // No action map, nothing to do
        return CE_OK;
    }

    // Just run the function, input maps do not do error checking
    inputComponent->m_actionMaps[inputComponent->m_currentActionMap](context, inputComponent, deltaTime, errorCode);
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION
