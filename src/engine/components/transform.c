//
//  engine/components/transform.c
//  A simple 2d Based transform component
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

void CE_TRANSFORM_COMPONENT_init(OUT CE_TransformComponent* component)
{
    component->x = 0;
    component->y = 0;
}

void CE_TRANSFORM_COMPONENT_cleanup(OUT CE_TransformComponent* component)
{
    // No dynamic resources to clean up for CE_TransformComponent
}

CE_GENERATE_COMPONENT_IMP(CE_TRANSFORM_COMPONENT, 10, CE_TransformComponent)