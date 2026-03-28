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
    debugComponent->m_ticked_rel = true;
    debugComponent->m_testValue += 1;
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_SCENE_ORDER, CE_CORE_TEST_SYSTEM_DEPENDENCIES)
{
    if (!CE_Entity_HasRelationship(context, entity, CE_RELATIONSHIP_PARENT))
    {
        debugComponent->m_testValue2 = 42;
    }
    else
    {
        CE_Id parentId = CE_INVALID_ID;
        if (CE_Entity_FindFirstRelationship(context, entity, CE_RELATIONSHIP_PARENT, &parentId, NULL) == CE_OK)
        {
            CE_Id componentId = CE_INVALID_ID;
            CE_Core_DebugComponent* parentDebugComponent = NULL;
            if (CE_Entity_FindFirstComponent(context, parentId, CE_CORE_DEBUG_COMPONENT, &componentId, (void**)&parentDebugComponent, NULL) == CE_OK)
            {
                debugComponent->m_testValue2 = parentDebugComponent->m_testValue2 + 1;
            }
        }
    }
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_SYSTEM_IMPLEMENTATION(CE_CORE_TEST_SYSTEM_NO_STORAGE, CE_CORE_TEST_SYSTEM_DEPENDENCIES_NO_STORAGE)
{
    (void)noStorageComponent;
    debugComponent->m_flagSystemTriggered = true;
}
CE_END_SYSTEM_IMPLEMENTATION

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_CORE_GLOBAL_TEST_SYSTEM)
{
    CE_ECS_AccessGlobalComponentToVariable(context, CE_CORE_GLOBAL_DEBUG_COMPONENT, globalDebugComp);
    if (!globalDebugComp)
    {
        CE_Error("Failed to access global debug component");
        return CE_ERROR;
    }

    if (globalDebugComp->m_enabled)
    {
#ifdef CE_CORE_TEST_MODE
        globalDebugComp->m_testValue++;
#endif
    }
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION

#endif // CE_CORE_TEST_MODE
