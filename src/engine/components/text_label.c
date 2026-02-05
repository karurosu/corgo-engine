//
//  engine/components/text_label.c
//  Text label component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_DEFINE_COMPONENT_INIT(CE_TEXT_LABEL_COMPONENT)
{
    component->text[0] = '\0';
    component->fontName = "";
    component->fontPtr = NULL;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TEXT_LABEL_COMPONENT)
{
    if (component->fontPtr)
    {
        CE_FREE_ASSET(CE_ASSET_TYPE_FONT, component->fontPtr);
        component->fontPtr = NULL;
    }
    return CE_OK;
}

// Helpers

CE_Result CE_TextLabelComponent_setText(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN const char* text)
{
    if (text == NULL || strlen(text) >= sizeof(component->text)) {
        return CE_ERROR;
    }

    if (strncpy(component->text, text, sizeof(component->text)) == NULL) {
        return CE_ERROR;
    }
    return CE_OK;
}

CE_Result CE_TextLabelComponent_setFont(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, IN const char* fontName)
{
    if (strcmp(component->fontName, fontName) == 0) {
        return CE_OK;
    }

    component->fontName = fontName;

    // Temp code to force loading of font
    if (component->fontPtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_FONT, component->fontPtr);
        component->fontPtr = NULL;
    }

    component->fontPtr = CE_CACHE_ASSET(context, CE_ASSET_TYPE_FONT, fontName, NULL);
    if (component->fontPtr == NULL)
    {
        return CE_ERROR;
    }

    return CE_OK;
}

CE_Result CE_TextLabelComponent_getTextBounds(INOUT CE_ECS_Context* context, INOUT CE_TextLabelComponent* component, OUT int* width, OUT int* height)
{
    if (component->fontPtr == NULL)
    {
        return CE_ERROR;
    }

#ifdef CE_BACKEND_PLAYDATE
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