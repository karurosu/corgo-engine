//
//  ecs/core/globals.h
//  Global consts that can be changed by the user, kept here for easy access
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_CONFIG_H
#define CORGO_ECS_CORE_CONFIG_H

//// Entity related
// Maximum number of entities that can exist at the same time
#define CE_MAX_ENTITIES 256

// Initial capacity for tracking entities and relationships
// Memory is dynamically allocated so this is the minimum initial capacity, larger numbers will reserve more memory
// Smaller numbers will use less memory initially but may require reallocations as more components are added
#define CE_INITIAL_ENTITY_COMPONENTS_CAPACITY 6
#define CE_INITIAL_ENTITY_RELATIONSHIPS_CAPACITY 3

//// Component related

// Helper const to set a defined initial capacity, simplifies increasing or decreasing capacity in the future
// In general this should match the max entities to guarantee enough space for at least one component per entity
#define CE_DEFAULT_COMPONENT_CAPACITY CE_MAX_ENTITIES

// If out of space, increase capacity by this amount
// Currently not used
#define CE_COMPONENT_GROWTH_AMOUNT 32

#endif // CORGO_ECS_CORE_CONFIG_H
