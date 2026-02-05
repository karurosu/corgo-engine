//
//  engine/components/transform.h
//  Transform component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_TRANSFORM_H
#define CORGO_ENGINE_COMPONENTS_TRANSFORM_H

#include "ecs/types.h"

//// Transform Component
typedef struct CETransformComponent {
    // Local position
    uint32_t x;
	uint32_t y;
    // Z-index for layering
	uint32_t z; 
    // Calculated world position based on parent relationships
    uint32_t world_x;
	uint32_t world_y;
} CE_TransformComponent;

CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN uint32_t x, IN uint32_t y, IN uint32_t z);

#endif // CORGO_ENGINE_COMPONENTS_TRANSFORM_H