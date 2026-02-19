//
//  engine/core/display.h
//  Functions to manipulate the display in a platform agnostic way.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_DISPLAY_H
#define CORGO_ENGINE_CORE_DISPLAY_H

#include "ecs/types.h"

typedef struct CE_DisplayComponent {
    int m_width;
    int m_height;
    uint8_t m_scale;
    uint8_t m_refreshRate;
    bool m_inverted;
    bool m_flipX;
    bool m_flipY;
} CE_DisplayComponent;

// Get display size used for rendering.
// Returns an int to simplify PD API integration.
#define CE_GetDisplayWidth(context)   (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_width)
#define CE_GetDisplayHeight(context)  (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_height)
#define CE_GetDisplayScale(context)   (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_scale)
#define CE_GetDisplayRefreshRate(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_refreshRate)
#define CE_IsDisplayInverted(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_inverted)
#define CE_IsDisplayFlippedX(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_flipX)
#define CE_IsDisplayFlippedY(context) (CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_flipY)

// Helper functions
void CE_Display_SetScale(INOUT CE_ECS_Context* context, IN uint8_t scale);
void CE_Display_SetRefreshRate(INOUT CE_ECS_Context* context, IN uint8_t refreshRate);
void CE_Display_SetInverted(INOUT CE_ECS_Context* context, IN bool inverted);
void CE_Display_SetFlipX(INOUT CE_ECS_Context* context, IN bool flipX);
void CE_Display_SetFlipY(INOUT CE_ECS_Context* context, IN bool flipY);
void CE_Display_Reset(INOUT CE_ECS_Context* context);

// Runtime functions
void CE_Display_Clear(INOUT CE_ECS_Context* context);

#endif // CORGO_ENGINE_CORE_DISPLAY_H