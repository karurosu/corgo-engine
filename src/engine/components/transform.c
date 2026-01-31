//
//  engine/components/transform.c
//  A simple 2d Based transform component
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

CE_DEFINE_COMPONENT_INIT(CE_TRANSFORM_COMPONENT)
{
    component->x = 0;
    component->y = 0;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TRANSFORM_COMPONENT)
{
    // No dynamic resources to clean up for CE_TransformComponent
    return CE_OK;
}

CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN uint32_t x, IN uint32_t y, IN uint32_t z)
{
    component->x = x;
    component->y = y;
    component->z = z;
    return CE_OK;
}


CE_GENERATE_COMPONENT_IMP(CE_TRANSFORM_COMPONENT)
