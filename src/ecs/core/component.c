//
//  ecs/core/component.c
//  Component methods
//  Copyright (c) 2026 Carlos Camacho.
//

#include "../components.h"

const char* CE_ECS_GetComponentTypeNameDebugStr(IN CE_TypeId typeId)
{
#ifdef CE_DEBUG_BUILD
    switch (typeId) {
#define X(name, uid, storage, initial_capacity) case name: return #name;
        CE_COMPONENT_DESC_CORE(X)
        CE_COMPONENT_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
        CE_COMPONENT_DESC_GAME(X)
#endif
    #undef X
        default: return "InvalidComponentType";
    }
#else
    return "NotInDebugBuild";
#endif
}
