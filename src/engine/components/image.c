//
//  engine/components/image.h
//  Image component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_COMPONENT_INIT(CE_IMAGE_COMPONENT)
{
    component->m_imagePtr = NULL;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_IMAGE_COMPONENT)
{
    if (component->m_imagePtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_BITMAP, component->m_imagePtr);
        component->m_imagePtr = NULL;
    }
    return CE_OK;
}

CE_Result CE_ImageComponent_setImage(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, IN CE_TransformComponent *transform, IN const char* imageName)
{
    if (component->m_imagePtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_BITMAP, component->m_imagePtr);
        component->m_imagePtr = NULL;
    }

    component->m_imagePtr = CE_CACHE_ASSET(context, CE_ASSET_TYPE_BITMAP, imageName, NULL);
    if (component->m_imagePtr == NULL)
    {
        return CE_ERROR;
    }
    
    return CE_ImageComponent_update(context, component, transform);
}

CE_Result CE_ImageComponent_clearImage(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, IN CE_TransformComponent *transform)
{
    if (component->m_imagePtr)
    {
        CE_RELEASE_ASSET(context, CE_ASSET_TYPE_BITMAP, component->m_imagePtr);
        component->m_imagePtr = NULL;
    }

    return CE_ImageComponent_update(context, component, transform);
}

CE_Result CE_ImageComponent_getImageBounds(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, OUT uint16_t* width, OUT uint16_t* height)
{
    if (!component->m_imagePtr)
    {
        *width = 0;
        *height = 0;
        return CE_OK;
    }
#ifdef CE_BACKEND_PLAYDATE
    int iwidth, iheight;
    CE_GetPlaydateAPI()->graphics->getBitmapData(component->m_imagePtr, &iwidth, &iheight, NULL, NULL, NULL);
    *width = (uint16_t)iwidth;
    *height = (uint16_t)iheight;
#else
    // TODO for other backedns
    *width = 0;
    *height = 0;
#endif
    return CE_OK;
}

CE_Result CE_ImageComponent_update(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, IN CE_TransformComponent *transform)
{
    CE_Engine_SceneGraph_MarkDirty(context);
    return CE_ImageComponent_getImageBounds(context, component, &transform->m_width, &transform->m_height);
}
