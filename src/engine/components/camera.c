//
//  engine/components/camera.c
//  Camera component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

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

CE_Result CE_Engine_Camera_SetPosition(INOUT CE_ECS_Context* context,IN int16_t x, IN int16_t y)
{
    CE_CameraComponent* cameraComponent = CE_ECS_AccessGlobalComponent(context, CE_ENGINE_CAMERA_COMPONENT);
    
    if (cameraComponent->m_x == x && cameraComponent->m_y == y) {
        // No change in position, do nothing
        return CE_OK;
    }

    cameraComponent->m_x = x;
    cameraComponent->m_y = y;

    // Mark the scene graph as dirty to ensure the new camera position is applied in the next render
    CE_Engine_SceneGraph_MarkDirty(context);

    return CE_OK;
}
