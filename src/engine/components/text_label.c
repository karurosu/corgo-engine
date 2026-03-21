//
//  engine/components/text_label.c
//  Text label component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_COMPONENT_INIT(CE_TEXT_LABEL_COMPONENT)
{
    component->m_staticTextPtr = NULL;
    component->m_fontName = "";
    component->m_fontPtr = NULL;
    component->m_inverted = false;
    cc_init(&component->m_text);
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TEXT_LABEL_COMPONENT)
{
    if (component->m_fontPtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_FONT, component->m_fontPtr);
        component->m_fontPtr = NULL;
    }
    cc_cleanup(&component->m_text);
    return CE_OK;
}

// Helpers

CE_Result CE_TextLabelComponent_setStaticText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* text)
{
    component->m_staticTextPtr = (char *)text;
    return CE_TextLabelComponent_update(context, component, transform);
}

CE_Result CE_TextLabelComponent_update(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform)
{
    CE_Engine_SceneGraph_MarkDirty(context);
    return CE_TextLabelComponent_getTextBounds(context, component, &transform->m_width, &transform->m_height);
}

CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN CE_TransformComponent *transform, IN const char* fontName)
{
    if (strcmp(component->m_fontName, fontName) == 0) {
        return CE_OK;
    }

    component->m_fontName = fontName;

    // Release old font
    if (component->m_fontPtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_FONT, component->m_fontPtr);
        component->m_fontPtr = NULL;
    }

    // Cache new font
    component->m_fontPtr = CE_CACHE_ASSET(context, CE_ASSET_TYPE_FONT, fontName, NULL);
    if (component->m_fontPtr == NULL)
    {
        return CE_ERROR;
    }

    return CE_TextLabelComponent_update(context, component, transform);
}

CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT uint16_t* width, OUT uint16_t* height)
{
    if (component->m_fontPtr == NULL || (component->m_staticTextPtr == NULL && cc_size(&component->m_text) == 0))
    {
        *width = 0;
        *height = 0;
        return CE_OK;
    }

#ifdef CE_BACKEND_PLAYDATE
    const char *text = component->m_staticTextPtr != NULL ? component->m_staticTextPtr : cc_first(&component->m_text);
    const int textLength = (int)(component->m_staticTextPtr != NULL ? strlen(component->m_staticTextPtr) : cc_size(&component->m_text));
    // The playdate API uses ints for text size
    *width = (uint16_t)CE_GetPlaydateAPI()->graphics->getTextWidth(component->m_fontPtr, text, textLength, kASCIIEncoding, 0);
    *height = (uint16_t)CE_GetPlaydateAPI()->graphics->getFontHeight(component->m_fontPtr);
#else
    // Stub implementation for non-Playdate backends
    *width = 0;
    *height = 0; 
#endif

    return CE_OK;
}

