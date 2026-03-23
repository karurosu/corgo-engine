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
#define CE_RELATIONSHIP_DESC(name, reciprocal) case name: return #name; case reciprocal: return #reciprocal;
	CE_RELATIONSHIP_DESC_CORE(CE_RELATIONSHIP_DESC)
	CE_RELATIONSHIP_DESC_ENGINE(CE_RELATIONSHIP_DESC)
#ifndef CE_CORE_TEST_MODE
	CE_RELATIONSHIP_DESC_GAME(CE_RELATIONSHIP_DESC)
#endif
#undef CE_RELATIONSHIP_DESC
        default: return "InvalidRelationshipType";
    }
#else
    return "NotInDebugBuild";
#endif
}   