//
//  engine/systems/input_pd.c
//  Playdate-specific input system.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

#ifdef CE_BACKEND_PLAYDATE

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_GLOBAL_INPUT_READER_PD_SYSTEM)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);
    // First check if we have to enable features
    if (inputComponent->m_requestedFeatures != inputComponent->m_enabledFeatures) {
        const uint32_t featuresToEnable = inputComponent->m_requestedFeatures & ~inputComponent->m_enabledFeatures;
        const uint32_t featuresToDisable = ~inputComponent->m_requestedFeatures & inputComponent->m_enabledFeatures;

        if (featuresToEnable & CE_Input_Feature_Accelerometer) {
            CE_GetPlaydateAPI()->system->setPeripheralsEnabled(kAccelerometer);
            CE_Debug("Enabled accelerometer input");
        }
        if (featuresToDisable & CE_Input_Feature_Accelerometer) {
            CE_GetPlaydateAPI()->system->setPeripheralsEnabled(kNone);
            CE_InputComponent_ResetAccelerometer(inputComponent);
            CE_Debug("Disabled accelerometer input");
        }
        
        inputComponent->m_enabledFeatures = inputComponent->m_requestedFeatures;
    }

    // Read raw button inputs
    CE_GetPlaydateAPI()->system->getButtonState((PDButtons*)&inputComponent->m_rawInputsCurrent, (PDButtons*)&inputComponent->m_rawInputsPressed, (PDButtons*)&inputComponent->m_rawInputsReleased);

    // Read accelerometer if enabled
    if (inputComponent->m_enabledFeatures & CE_Input_Feature_Accelerometer) {
        CE_GetPlaydateAPI()->system->getAccelerometer(&inputComponent->m_accelerometerX, &inputComponent->m_accelerometerY, &inputComponent->m_accelerometerZ);
    }

    // Read crank if enabled
    if (inputComponent->m_enabledFeatures & CE_Input_Feature_Crank) {
        inputComponent->m_crankAngle = CE_GetPlaydateAPI()->system->getCrankAngle();

        // Check if crank is docked
        const bool isUndocked = inputComponent->m_rawInputsCurrent & CE_Input_Crank;
        const bool isUndockedPD = !CE_GetPlaydateAPI()->system->isCrankDocked();
        if (isUndockedPD != isUndocked) {
            if (isUndockedPD) {
                inputComponent->m_rawInputsCurrent |= CE_Input_Crank;
                inputComponent->m_rawInputsPressed |= CE_Input_Crank; // Consider undock as a "press" event
            } else {
                inputComponent->m_rawInputsCurrent &= ~CE_Input_Crank;
                inputComponent->m_rawInputsReleased |= CE_Input_Crank; // Consider dock as a "release" event
            }
        }
        else
        {
            // If dock state hasn't changed, clear pressed/released for the dock bit
            inputComponent->m_rawInputsPressed &= ~CE_Input_Crank;
            inputComponent->m_rawInputsReleased &= ~CE_Input_Crank;
        }
    }
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION

#endif // CE_BACKEND_PLAYDATE