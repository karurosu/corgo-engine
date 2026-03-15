//
//  engine/systems/input_stub.c
//  Stub system for input handling.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

#ifndef CE_BACKEND_PLAYDATE

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_GLOBAL_INPUT_READER_STUB_SYSTEM)
{
    // Stub does nothing, input will not be updated.
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION

#endif // CE_BACKEND_PLAYDATE