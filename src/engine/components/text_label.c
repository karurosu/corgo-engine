//
//  engine/components/text_label.c
//  A simple 2d Based transform component
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

#include "helpers.h"
#include "engine/core/memory.h"

CE_DEFINE_COMPONENT_INIT(CE_TEXT_LABEL_COMPONENT)
{
    component->text[0] = '\0';
    component->font[0] = '\0';
#ifdef CE_BACKEND_PLAYDATE
    component->fontPtr = NULL;
#endif
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TEXT_LABEL_COMPONENT)
{
#ifdef CE_BACKEND_PLAYDATE
    if (component->fontPtr)
    {
        CE_free(component->fontPtr);
        component->fontPtr = NULL;
    }
#endif
    return CE_OK;
}

// Helpers

CE_Result CE_TextLabelComponent_setText(INOUT CE_TextLabelComponent* component, IN const char* text)
{
    if (text == NULL || strlen(text) >= sizeof(component->text)) {
        return CE_ERROR;
    }

    if (strncpy(component->text, text, sizeof(component->text)) == NULL) {
        return CE_ERROR;
    }
    return CE_OK;
}

CE_Result CE_TextLabelComponent_setFont(INOUT CE_TextLabelComponent* component, IN const char* fontName)
{
    if (fontName == NULL || strlen(fontName) >= sizeof(component->font)) {
        return CE_ERROR;
    }

    if (strncpy(component->font, fontName, sizeof(component->font)) == NULL) {
        return CE_ERROR;
    }

#ifdef CE_BACKEND_PLAYDATE
    // Temp code to force loading of font
    if (component->fontPtr)
    {
        CE_free(component->fontPtr);
        component->fontPtr = NULL;
    }

    const char* err;
    component->fontPtr = CE_GetPlaydateAPI()->graphics->loadFont(component->font, &err);
    if (component->fontPtr == NULL)
    {
        CE_Error("Failed to load font %s for TextLabelComponent: %s", component->font, err);
        return CE_ERROR;
    }
#endif

    return CE_OK;
}

CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_TextLabelComponent* component, OUT int* width, OUT int* height)
{
#ifdef CE_BACKEND_PLAYDATE
    if (component->fontPtr == NULL)
    {
        return CE_ERROR;
    }

    *width = CE_GetPlaydateAPI()->graphics->getTextWidth(component->fontPtr, component->text, sizeof(component->text), kASCIIEncoding, 0);
    *height = CE_GetPlaydateAPI()->graphics->getFontHeight(component->fontPtr);
#else
    // Stub implementation for non-Playdate backends
    *width = 0;
    *height = 0; 
#endif
    return CE_OK;
}

CE_GENERATE_COMPONENT_IMP(CE_TEXT_LABEL_COMPONENT)  