//
//  engine/components/transform.c
//  A simple 2d Based transform component
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

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

CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int32_t x, IN int32_t y, IN uint16_t z)
{
    component->x = x;
    component->y = y;
    component->z = z;
    CE_Engine_SceneGraph_MarkDirty(context);
    CE_Engine_SceneGraph_MarkZOrderDirty(context);
    return CE_OK;
}

CE_Result CE_TransformComponent_setPositionXY(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int32_t x, IN int32_t y)
{
    component->x = x;
    component->y = y;
    CE_Engine_SceneGraph_MarkDirty(context);
    return CE_OK;
}

CE_Result CE_TransformComponent_setZIndex(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN uint16_t z)
{
    component->z = z;
    CE_Engine_SceneGraph_MarkDirty(context);
    CE_Engine_SceneGraph_MarkZOrderDirty(context);
    return CE_OK;
}


CE_GENERATE_COMPONENT_IMP(CE_TRANSFORM_COMPONENT)
