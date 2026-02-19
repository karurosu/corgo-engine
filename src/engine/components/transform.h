//
//  engine/components/transform.h
//  Transform component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_TRANSFORM_H
#define CORGO_ENGINE_COMPONENTS_TRANSFORM_H

#include "ecs/types.h"

//// Transform Component
typedef struct CE_TransformComponent {
    // Local position
    int16_t m_x;
	int16_t m_y;
    // Z-index for layering
	int16_t m_z;
    // Size
    uint16_t m_width;
    uint16_t m_height;
    bool m_inSceneGraph;
} CE_TransformComponent;

/// Public API

/**
 * @brief Set the position (X, Y) of the TransformComponent.
 * This must be called to update the values to guarantee the scene graph is updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param component[in,out] The TransformComponent to update.
 * @param x[in] The new X position.
 * @param y[in] The new Y position.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t x, IN int16_t y);

/**
 * @brief Set the Z-index of the TransformComponent.
 * This must be called to update the value to guarantee the scene graph is updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param component[in,out] The TransformComponent to update.
 * @param z[in] The new Z-index.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TransformComponent_setZIndex(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t z);

#endif // CORGO_ENGINE_COMPONENTS_TRANSFORM_H