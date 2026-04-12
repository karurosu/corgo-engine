//
//  engine/systems/image.c
//  Systems that deal with images.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_IMAGE_RENDERER, CE_IMAGE_SYSTEM_DEPENDENCIES)
{
#ifdef CE_BACKEND_PLAYDATE
    CE_SceneGraphRenderNode* renderNode = CE_Scene_GetRenderNode(context, entity);
    if (renderNode == NULL) {
        return CE_OK; // Entity not in scene graph, skip rendering
    }

    if (imageComponent->m_imagePtr == NULL)
    {
        return CE_OK; // Image not set yet, skip rendering
    }
    
    CE_GetPlaydateAPI()->graphics->drawBitmap(imageComponent->m_imagePtr, renderNode->m_x, renderNode->m_y, kBitmapUnflipped);
#endif
}
CE_END_SYSTEM_IMPLEMENTATION
