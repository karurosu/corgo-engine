//
//  engine/components/camera.c
//  Camera component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_CAMERA_COMPONENT)
{
    component->m_x = 0;
    component->m_y = 0;
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_CAMERA_COMPONENT)
{
    return CE_OK;
}
