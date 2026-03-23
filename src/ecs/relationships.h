//
//  ecs/relationships.h
//  Includes the list of all relationship types, including core, engine, and game-defined relationships.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_RELATIONSHIPS_H
#define CORGO_ECS_RELATIONSHIPS_H

#include "types.h"

//// Build relationship type list
// Core ECS relationships
#include "core/core_relationships.h"
#ifndef CE_RELATIONSHIP_DESC_CORE
#define CE_RELATIONSHIP_DESC_CORE(CE_RELATIONSHIP_DESC)
#endif

// Engine relationships
#include "../engine/relationships.h"
#ifndef CE_RELATIONSHIP_DESC_ENGINE
#define CE_RELATIONSHIP_DESC_ENGINE(CE_RELATIONSHIP_DESC)
#endif

// Game relationships
#include "../game/relationships.h"
#ifndef CE_RELATIONSHIP_DESC_GAME
#define CE_RELATIONSHIP_DESC_GAME(CE_RELATIONSHIP_DESC)
#endif

//// Generate relationship Types Enum
typedef enum CE_RELATIONSHIP_TYPES_ENUM {
#define CE_RELATIONSHIP_DESC(relationship, reciprocal) relationship, reciprocal,
	CE_RELATIONSHIP_DESC_CORE(CE_RELATIONSHIP_DESC)
	CE_RELATIONSHIP_DESC_ENGINE(CE_RELATIONSHIP_DESC)
#ifndef CE_CORE_TEST_MODE
	CE_RELATIONSHIP_DESC_GAME(CE_RELATIONSHIP_DESC)
#endif
#undef CE_RELATIONSHIP_DESC
	CE_RELATIONSHIP_TYPES_COUNT //Invalid system count
} CE_RELATIONSHIP_TYPES;

static const int CE_RELATIONSHIPS_RECIPROCALS[CE_RELATIONSHIP_TYPES_COUNT] = {
#define CE_RELATIONSHIP_DESC(relationship, reciprocal) reciprocal, relationship,
	CE_RELATIONSHIP_DESC_CORE(CE_RELATIONSHIP_DESC)
	CE_RELATIONSHIP_DESC_ENGINE(CE_RELATIONSHIP_DESC)
#ifndef CE_CORE_TEST_MODE
	CE_RELATIONSHIP_DESC_GAME(CE_RELATIONSHIP_DESC)
#endif
#undef CE_RELATIONSHIP_DESC
};

//// Debug helpers
const char* CE_ECS_GetRelationshipTypeNameDebugStr(IN CE_TypeId typeId);

#endif