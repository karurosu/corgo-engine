//
//  ecs/core/entity.h
//  Main include file for all entity definitions.
//  All functions here are internal to the ECS implementation.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_ENTITY_H
#define CORGO_ECS_CORE_ENTITY_H

#include "../types.h"

// Entity data definitions
typedef struct CE_ECS_EntityData {
    CE_Id m_entityId;
    CE_Bitset m_entityComponentBitset; // Bitset to track which component types are attached to the entity
    CE_Bitset m_entityRelationshipBitset; // Bitset to track which relationship types are attached to the entity
    CE_Id_Set m_components; // Vector of component ids attached to the entity
    CE_Id_Set m_relationships; // Vector of relationship ids attached to the entity
} CE_ECS_EntityData;

#endif // CORGO_ECS_CORE_ENTITY_H
