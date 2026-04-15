//
//  engine/components/camera.h
//  Camera component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//
#ifndef CORGO_ENGINE_COMPONENTS_CAMERA_H
#define CORGO_ENGINE_COMPONENTS_CAMERA_H

#include "ecs/types.h"

typedef struct CE_CameraComponent {
    // Position
    int16_t m_x;
    int16_t m_y;
} CE_CameraComponent;

/**
 * Sets the camera position. The camera's position determines the offset applied to all rendered entities, allowing for scrolling and panning effects.
 * Use this to guarantee that the screen is updated
 * 
 * @param context[inout] Pointer to the ECS context.
 * @param x[in] The new X position of the camera.
 * @param y[in] The new Y position of the camera.
 * 
 * @return CE_OK if the camera position was successfully updated, or an appropriate error code if the operation failed.
 */
CE_Result CE_Engine_Camera_SetPosition(INOUT CE_ECS_Context* context,IN int16_t x, IN int16_t y);

/**
 * Function like macro to get the current X position of the camera.
 * 
 * @param context[in] Pointer to the ECS context.
 * 
 * @return The current X position of the camera as a uint16_t
 */
#define CE_Engine_Camera_GetX(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_CAMERA_COMPONENT)->m_x);

/**
 * Function like macro to get the current Y position of the camera.
 * 
 * @param context[in] Pointer to the ECS context.
 * 
 * @return The current Y position of the camera as a uint16_t
 */
#define CE_Engine_Camera_GetY(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_CAMERA_COMPONENT)->m_y);

#endif // CORGO_ENGINE_COMPONENTS_CAMERA_H