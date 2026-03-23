//
//  engine/sample_input/sample.c
//  Sample input maps.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/input_map.h"

CE_DECLARE_ACTION_MAP_FUNCTION(SampleInputMap1)
{
    CES_ON_PRESS(CE_Input_ButtonA, DEMO_PRESS);
    CES_ON_RELEASE(CE_Input_ButtonA, DEMO_RELEASE);
    CES_WHILE_HELD(CE_Input_ButtonA, DEMO_HELD);

    CES_WHILE_HELD(CE_Input_ButtonA | CE_Input_ButtonB, DEMO_HELD_2);

    CES_ON_PRESS(CE_Input_ButtonUp, NEXT_INPUT_MAP);
    CES_ON_PRESS(CE_Input_ButtonDown, PREVIOUS_INPUT_MAP);
}

CE_DECLARE_ACTION_MAP_FUNCTION(SampleInputMap2)
{
    CES_ON_PRESS(CE_Input_ButtonB, DEMO_PRESS);
    CES_ON_RELEASE(CE_Input_ButtonB, DEMO_RELEASE);
    CES_WHILE_HELD(CE_Input_ButtonB, DEMO_HELD);

    CES_WHILE_HELD(CE_Input_ButtonA | CE_Input_ButtonB, DEMO_HELD_2);

    CES_ON_PRESS(CE_Input_ButtonDown, PREVIOUS_INPUT_MAP);
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
