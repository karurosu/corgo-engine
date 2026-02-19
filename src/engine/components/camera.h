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

#endif // CORGO_ENGINE_COMPONENTS_CAMERA_H