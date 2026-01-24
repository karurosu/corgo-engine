//
//  engine/components/helpers.h
//  Helper functions for maniuplating engine components.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_HELPERS_H
#define CORGO_ENGINE_COMPONENTS_HELPERS_H

#include "ecs/ecs.h"

/// Transform Component Helpers (Implementation in transform.c)
CE_Result CE_TransformComponent_setPosition(INOUT CE_TransformComponent* component, IN uint32_t x, IN uint32_t y, IN uint32_t z);


/// Sprite Component Helpers (Implementation in sprite.c)


/// Text Label Component Helpers (Implementation in text_label.c)
CE_Result CE_TextLabelComponent_setText(INOUT CE_TextLabelComponent* component, IN const char* text);
CE_Result CE_TextLabelComponent_setFont(INOUT CE_TextLabelComponent* component, IN const char* fontName);
CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_TextLabelComponent* component, OUT int* width, OUT int* height);


#endif // CORGO_ENGINE_COMPONENTS_HELPERS_H