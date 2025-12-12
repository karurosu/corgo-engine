//
//  ecs/core/globals.h
//  Global consts that can be changed by the user, kept here for easy access
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_CONFIG_H
#define CORGO_ECS_CORE_CONFIG_H

//// Component related

// If out of space, increase capacity by this amount
#define CE_COMPONENT_GROWTH_AMOUNT 32

// Helper const to set a defined initial capacity, simplifies increasing or decreasing capacity in the future
// Only use if the component will be in every entity as it will always be allocated
#define CE_DEFAULT_COMPONENT_CAPACITY 64


#endif // CORGO_ECS_CORE_CONFIG_H
