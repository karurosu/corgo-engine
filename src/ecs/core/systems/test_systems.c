//
//  ecs/core/systems/test_systems.c
//  Systems used for unit testing.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifdef CE_CORE_TEST_MODE
#include "ecs/ecs.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_DISPLAY, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)
{
    debugComponent->m_ticked_display = true;
    debugComponent->m_testValue += 1;
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_HALF_DISPLAY, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)
{
    debugComponent->m_ticked_half = true;
    debugComponent->m_testValue += 1;
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_SECOND, CE_CORE_DEBUG_SYSTEM_DEPENDENCIES)
{
    debugComponent->m_ticked_second = true;
    debugComponent->m_testValue += 1;
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_REL, CE_CORE_TEST_SYSTEM_DEPENDENCIES_REL)
{
    debugComponent->m_testValue += 1;
}
CE_END_SYSTEM_IMPLEMENTATION

#endif // CE_CORE_TEST_MODE
