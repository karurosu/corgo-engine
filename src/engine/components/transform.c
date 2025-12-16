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

CE_GENERATE_COMPONENT_IMP(CE_TRANSFORM_COMPONENT)
