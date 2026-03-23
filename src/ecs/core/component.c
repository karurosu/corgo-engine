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
#define CE_COMPONENT_DESC(name, uid, storage, initial_capacity, ...) case name: return #name;
        CE_COMPONENT_DESC_CORE(CE_COMPONENT_DESC)
        CE_COMPONENT_DESC_ENGINE(CE_COMPONENT_DESC)
#ifndef CE_CORE_TEST_MODE
        CE_COMPONENT_DESC_GAME(CE_COMPONENT_DESC)
#endif
    #undef CE_COMPONENT_DESC
        default: return "InvalidComponentType";
    }
#else
    return "NotInDebugBuild";
#endif
}

const char* CE_ECS_GetGlobalComponentTypeNameDebugStr(IN CE_TypeId typeId)
{
#ifdef CE_DEBUG_BUILD
    switch (typeId) {
#define CE_GLOBAL_COMPONENT_DESC(name, storage) case CE_GLOBAL_COMPONENT(name): return #name;
        CE_GLOBAL_COMPONENT_DESC_CORE(CE_GLOBAL_COMPONENT_DESC)
        CE_GLOBAL_COMPONENT_DESC_ENGINE(CE_GLOBAL_COMPONENT_DESC)
#ifndef CE_CORE_TEST_MODE
        CE_GLOBAL_COMPONENT_DESC_GAME(CE_GLOBAL_COMPONENT_DESC)
#endif
    #undef CE_GLOBAL_COMPONENT_DESC
        default: return "InvalidGlobalComponentType";
    }
#else
    return "NotInDebugBuild";
#endif
}
