//
//  engine/components/text_label.h
//  Text label component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H
#define CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H

#include "ecs/types.h"
#include "engine/assets.h"

//// Text Label Component
typedef struct CETextLabelComponent {
	char text[256];
	const char *fontName;
	CE_ASSET_PTR(CE_ASSET_TYPE_FONT) fontPtr;
} CE_TextLabelComponent;

// Helpers
CE_Result CE_TextLabelComponent_setText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN const char* text);
CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN const char* fontName);
CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT int* width, OUT int* height);

#endif // CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H