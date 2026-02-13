//
//  engine/systems/text_label_system.c
//  Systems that deal with text labels.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"
#include "engine/core/platform.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_TEXT_LABEL_SYSTEM, CE_TEXT_LABEL_SYSTEM_DEPENDENCIES)
{
#ifdef CE_BACKEND_PLAYDATE
    if (textLabelComponent->fontPtr == NULL)
    {
        return CE_OK; // Font not set yet, skip rendering
    }
    LCDBitmapDrawMode previousDrawMode = kDrawModeCopy;
    if (textLabelComponent->m_inverted) {
        previousDrawMode = CE_GetPlaydateAPI()->graphics->setDrawMode(kDrawModeFillWhite);
    } else {
        previousDrawMode = CE_GetPlaydateAPI()->graphics->setDrawMode(kDrawModeCopy);
    }
    CE_GetPlaydateAPI()->graphics->setFont(textLabelComponent->fontPtr);
    CE_GetPlaydateAPI()->graphics->drawText(textLabelComponent->text, strlen(textLabelComponent->text), kASCIIEncoding, transformComponent->x, transformComponent->y);
    CE_GetPlaydateAPI()->graphics->setDrawMode(previousDrawMode);
#endif
}
CE_END_SYSTEM_IMPLEMENTATION

