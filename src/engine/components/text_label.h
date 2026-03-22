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
	char *m_staticTextPtr;
	cc_str(char) m_text;
	CE_ASSET_PTR(CE_ASSET_TYPE_FONT) m_fontPtr;
} CE_TextLabelComponent;

typedef struct CE_TransformComponent CE_TransformComponent;

// Helpers
/**
 * @brief Set the text of a text label and update its bounds.
 * This should only be called for static strings (i.e. string literals) or text that is manually managed.
 * 
 * @param[in,out] context The ECS context.
 * @param[in,out] component The text label component to update.
 * @param[in] transform The transform component to update bounds on.
 * @param[in] text The new text to set.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 **/
CE_Result CE_TextLabelComponent_setStaticText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* text);

/**
 * @brief Set the font of a text label and update its bounds.
 * 
 * @param[in,out] context The ECS context.
 * @param[in,out] component The text label component to update.
 * @param[in] transform The transform component to update bounds on.
 * @param[in] fontName The name of the font to set.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* fontName);

/**
 * @brief Get the bounds of the text label.
 * 
 * @param[in,out] context The ECS context.
 * @param[in,out] component The text label component to query.
 * @param[out] width The width of the text label.
 * @param[out] height The height of the text label.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT uint16_t* width, OUT uint16_t* height);

/**
 * @brief Update the text label component. Must be called after manipulating the dynamic text.
 * 
 * @param[in,out] context The ECS context.
 * @param[in,out] component The text label component to update.
 * @param[in] transform The transform component to update bounds on.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TextLabelComponent_update(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform);

#endif // CORGO_ENGINE_COMPONENTS_TEXT_LABEL_H
