//
//  engine/components/text_label.c
//  Text label component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_DEFINE_COMPONENT_INIT(CE_TEXT_LABEL_COMPONENT)
{
    component->m_text[0] = '\0';
    component->m_fontName = "";
    component->m_fontPtr = NULL;
    component->m_inverted = false;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TEXT_LABEL_COMPONENT)
{
    if (component->m_fontPtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_FONT, component->m_fontPtr);
        component->m_fontPtr = NULL;
    }
    return CE_OK;
}

// Helpers

CE_Result CE_TextLabelComponent_setText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* text)
{
    if (text == NULL || strlen(text) >= sizeof(component->m_text)) {
        return CE_ERROR;
    }

    if (strncpy(component->m_text, text, sizeof(component->m_text)) == NULL) {
        return CE_ERROR;
    }

    if (component->m_fontPtr != NULL)
    {
        // Update bounds based on new text
        CE_TextLabelComponent_getTextBounds(context, component, &transform->m_width, &transform->m_height);
        return CE_OK;
    }

    return CE_OK;
}

CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* fontName)
{
    if (strcmp(component->m_fontName, fontName) == 0) {
        return CE_OK;
    }

    component->m_fontName = fontName;

    // Temp code to force loading of font
    if (component->m_fontPtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_FONT, component->m_fontPtr);
        component->m_fontPtr = NULL;
    }

    component->m_fontPtr = CE_CACHE_ASSET(context, CE_ASSET_TYPE_FONT, fontName, NULL);
    if (component->m_fontPtr == NULL)
    {
        return CE_ERROR;
    }

    if (component->m_fontPtr != NULL)
    {
        // Update bounds based on new font
        CE_TextLabelComponent_getTextBounds(context, component, &transform->m_width, &transform->m_height);
        return CE_OK;
    }

    return CE_OK;
}

CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT uint16_t* width, OUT uint16_t* height)
{
    if (component->m_fontPtr == NULL)
    {
        return CE_ERROR;
    }

#ifdef CE_BACKEND_PLAYDATE
    // The playdate API uses ints, that we know will fit in uint16_t since the display is smaller than that, so we can safely cast here.
    *width = (uint16_t)CE_GetPlaydateAPI()->graphics->getTextWidth(component->m_fontPtr, component->m_text, sizeof(component->m_text), kASCIIEncoding, 0);
    *height = (uint16_t)CE_GetPlaydateAPI()->graphics->getFontHeight(component->m_fontPtr);
#else
    // Stub implementation for non-Playdate backends
    *width = 0;
    *height = 0; 
#endif

    return CE_OK;
}

CE_GENERATE_COMPONENT_IMP(CE_TEXT_LABEL_COMPONENT)  