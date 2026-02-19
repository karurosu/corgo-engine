//
//  engine/components/text_label.h
//  Text label component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H
#define CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H

#include "ecs/types.h"
#include "engine/assets.h"

#define CE_ENGINE_TEXT_LABEL_BUFFER_SIZE 255

//// Text Label Component
typedef struct CETextLabelComponent {
	bool m_inverted;
	char m_text[CE_ENGINE_TEXT_LABEL_BUFFER_SIZE];
	const char *m_fontName;
	CE_ASSET_PTR(CE_ASSET_TYPE_FONT) m_fontPtr;
} CE_TextLabelComponent;

typedef struct CE_TransformComponent CE_TransformComponent;

// Helpers
CE_Result CE_TextLabelComponent_setText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* text);
CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* fontName);
CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT uint16_t* width, OUT uint16_t* height);

#endif // CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H