//
//  ecs/core/relationship.c
//  Code for manipulating relationships.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "../relationships.h"

const char* CE_ECS_GetRelationshipTypeNameDebugStr(IN CE_TypeId typeId)
{
#ifdef CE_DEBUG_BUILD
    switch (typeId) {
#define X(name) case name: return #name;
	CE_RELATIONSHIP_DESC_CORE(X)
	CE_RELATIONSHIP_DESC_ENGINE(X)
#ifndef CE_CORE_TEST_MODE
	CE_RELATIONSHIP_DESC_GAME(X)
#endif
#undef X
        default: return "InvalidRelationshipType";
    }
#else
    return "NotInDebugBuild";
#endif
}   