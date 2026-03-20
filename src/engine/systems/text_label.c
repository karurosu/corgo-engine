//
//  engine/systems/text_label.c
//  Systems that deal with text labels.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_TEXT_LABEL_RENDERER, CE_TEXT_LABEL_SYSTEM_DEPENDENCIES)
{
#ifdef CE_BACKEND_PLAYDATE
    CE_SceneGraphRenderNode* renderNode = CE_Scene_GetRenderNode(context, entity);
    if (renderNode == NULL) {
        return CE_OK; // Entity not in scene graph, skip rendering
    }

    if (textLabelComponent->m_fontPtr == NULL)
    {
        return CE_OK; // Font not set yet, skip rendering
    }

    LCDBitmapDrawMode previousDrawMode = kDrawModeCopy;
    if (textLabelComponent->m_inverted) {
        previousDrawMode = CE_GetPlaydateAPI()->graphics->setDrawMode(kDrawModeFillWhite);
    } else {
        previousDrawMode = CE_GetPlaydateAPI()->graphics->setDrawMode(kDrawModeCopy);
    }

    const char *text = textLabelComponent->m_staticTextPtr != NULL ? textLabelComponent->m_staticTextPtr : cc_first(&textLabelComponent->m_text);
    const int textLength = (int)(textLabelComponent->m_staticTextPtr != NULL ? strlen(textLabelComponent->m_staticTextPtr) : cc_size(&textLabelComponent->m_text));

    CE_GetPlaydateAPI()->graphics->setFont(textLabelComponent->m_fontPtr);
    CE_GetPlaydateAPI()->graphics->drawText(text, textLength, kASCIIEncoding, renderNode->m_x, renderNode->m_y);
    CE_GetPlaydateAPI()->graphics->setDrawMode(previousDrawMode);
#endif
}
CE_END_SYSTEM_IMPLEMENTATION

