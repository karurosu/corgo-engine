//
//  engine/systems/text_label.c
//  Systems that deal with text labels.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"
#include "engine/core/platform.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_TEXT_LABEL_RENDERER, CE_TEXT_LABEL_SYSTEM_DEPENDENCIES)
{
#ifdef CE_BACKEND_PLAYDATE
    CE_SceneGraphRenderNode* renderNode = CE_SceneGraph_GetRenderNode(context, entity);
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
    CE_GetPlaydateAPI()->graphics->setFont(textLabelComponent->m_fontPtr);
    CE_GetPlaydateAPI()->graphics->drawText(textLabelComponent->m_text, strlen(textLabelComponent->m_text), kASCIIEncoding, renderNode->m_x, renderNode->m_y);
    CE_GetPlaydateAPI()->graphics->setDrawMode(previousDrawMode);
#endif
}
CE_END_SYSTEM_IMPLEMENTATION

