//
//  engine/components/transform.c
//  A simple 2d Based transform component
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_COMPONENT_INIT(CE_TRANSFORM_COMPONENT)
{
    component->m_x = 0;
    component->m_y = 0;
    component->m_z = 0;
    component->m_width = 0;
    component->m_height = 0;
    component->m_flags = CE_TransformComponent_Flags_None;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TRANSFORM_COMPONENT)
{   
    const CE_Id entityId = CE_GetCurrentEntityId(context);
    if (CE_TransformComponent_isInSceneGraph(component) && entityId != CE_INVALID_ID) { // Entity Id may be null during program termination
        if (CE_Engine_SceneGraph_DeleteRenderNode(context, entityId) != CE_OK) {
            CE_Error("Failed to remove TransformComponent of entity %u from scene graph during cleanup", entityId);
            return CE_ERROR;
        }
    }
    return CE_OK;
}

CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t x, IN int16_t y)
{
    if (component->m_x != x || component->m_y != y) {
        CE_Engine_SceneGraph_MarkDirty(context);
        component->m_x = x;
        component->m_y = y;
    }
    return CE_OK;
}

CE_Result CE_TransformComponent_setZIndex(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t z)
{
    if (component->m_z != z) {
        CE_Engine_SceneGraph_MarkDirty(context);
        component->m_z = z;
    }
    return CE_OK;
}

