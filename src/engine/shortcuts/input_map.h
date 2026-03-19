//
//  engine/shortcuts/input_map.h
//  These are macro shortcuts to make it easier to write input map scripts, they are optional to use but can make code more legible.
//  These may not work outside of input map scripts.
//  Shortcuts are not included by default.
//  Using CES_ prefix to separate them from engine API, CES = Corgo Engine Shortcuts.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_SHORTCUTS_INPUT_MAP_H
#define CORGO_ENGINE_SHORTCUTS_INPUT_MAP_H

#define CES_ON_PRESS(button, action) \
    if ((inputComponent->m_rawInputsPressed & (button)) == (button)) {\
        inputComponent->m_actionStates |= (CE_IA_MASK(action));\
    }\


#define CES_ON_RELEASE(button, action) \
    if ((inputComponent->m_rawInputsReleased & (button)) == (button)) {\
        inputComponent->m_actionStates |= (CE_IA_MASK(action));\
    }\

    
#define CES_WHILE_HELD(button, action) \
    if ((inputComponent->m_rawInputsCurrent & (button)) == (button)) {\
        inputComponent->m_actionStates |= (CE_IA_MASK(action));\
    }\



#endif // CORGO_ENGINE_SHORTCUTS_INPUT_MAP_H
