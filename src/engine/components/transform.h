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
    int32_t x;
	int32_t y;
    // Z-index for layering
	uint16_t z;
} CE_TransformComponent;

CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int32_t x, IN int32_t y, IN uint16_t z);
CE_Result CE_TransformComponent_setPositionXY(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int32_t x, IN int32_t y);
CE_Result CE_TransformComponent_setZIndex(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN uint16_t z);

#endif // CORGO_ENGINE_COMPONENTS_TRANSFORM_H