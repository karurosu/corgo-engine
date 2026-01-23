//
//  ecs/core/system.c
//  Code for manipulating systems.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "system.h"

#include "ecs/components.h"
#include "ecs/systems.h"
#include "ecs/relationships.h"

#undef REQUIRE_COMPONENT
#define REQUIRE_COMPONENT(componentType, varName) \
    CE_Bitset_setBit(&data->m_requiredComponentBitset, componentType);

#define CE_GENERATE_SYSTEM_DESCRIPTION_FUNCTION(name, ...) \
void name##_description(OUT CE_ECS_SystemStaticData *data) \
{\
    data->m_systemId = name;\
    data->m_runOrder = name##_runOrder;\
    data->m_runPhase = name##_runPhase;\
    data->m_runFrequency = name##_runFrequency;\
    data->m_runFunction = name##_run;\
    CE_Bitset_init(&data->m_requiredComponentBitset, CE_COMPONENT_TYPES_COUNT);\
    CE_Bitset_init(&data->m_requiredRelationshipBitset, CE_RELATIONSHIP_TYPES_COUNT);\
    data->m_isValid = true;\
    data->m_enabled = true;\
    __VA_ARGS__ \
}

#define X(name, run_order, run_phase, run_frequency, ...) CE_GENERATE_SYSTEM_DESCRIPTION_FUNCTION(name, __VA_ARGS__)
	CE_SYSTEM_DESC_CORE(X)
	CE_SYSTEM_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_SYSTEM_DESC_GAME(X)
#endif
#undef X

#undef REQUIRE_COMPONENT

const char* CE_ECS_GetSystemTypeNameDebugStr(IN CE_TypeId typeId)
{
#ifdef CE_DEBUG_BUILD
    switch (typeId) {
#define X(name, run_order, run_phase, run_frequency, ...) case name: return #name;
        CE_SYSTEM_DESC_CORE(X)
        CE_SYSTEM_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
        CE_SYSTEM_DESC_GAME(X)
#endif
    #undef X
        default: return "InvalidSystemType";
    }
#else
    return "NotInDebugBuild";
#endif
}
