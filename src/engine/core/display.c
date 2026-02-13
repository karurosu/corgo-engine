//
//  engine/core/display.c
//  Functions to manipulate the display in a platform agnostic way.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_DISPLAY_COMPONENT)
{
    component->m_inverted = false;
    component->m_flipX = false;
    component->m_flipY = false;
    component->m_scale = CE_ENGINE_SCALE_DEFAULT;
    component->m_refreshRate = CE_ENGINE_REFRESH_RATE_DEFAULT;

#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setScale(CE_ENGINE_SCALE_DEFAULT);
    component->m_width = pd->display->getWidth();
    component->m_height = pd->display->getHeight();
    pd->display->setRefreshRate((float)component->m_refreshRate);
#else
    component->m_width = 400;
    component->m_height = 240;
#endif
    
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_DISPLAY_COMPONENT)
{
    return CE_OK;
}

void CE_Display_SetScale(INOUT CE_ECS_Context* context, IN uint8_t scale)
{   
    if (scale != 1 && scale != 2 && scale != 4 && scale != 8) {
        CE_Error("Invalid display scale: %d. Valid values are 1, 2, 4, or 8.", scale);
        return;
    }
    
    CE_Debug("Setting display scale to %d", scale);
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_DISPLAY_COMPONENT, displayComp);
    displayComp->m_scale = scale;
#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setScale(scale);
    displayComp->m_width = pd->display->getWidth();
    displayComp->m_height = pd->display->getHeight();
#endif
}

void CE_Display_SetRefreshRate(INOUT CE_ECS_Context* context, IN uint8_t refreshRate)
{
    CE_Debug("Setting display refresh rate to %d Hz", refreshRate > 50 ? 50 : refreshRate);
    CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_refreshRate = refreshRate > 50 ? 50 : refreshRate;
#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setRefreshRate((float)refreshRate);
#endif
}

void CE_Display_SetInverted(INOUT CE_ECS_Context* context, IN bool inverted)
{
    CE_Debug("Setting display inverted to: %s", inverted ? "true" : "false");
    CE_ECS_AccessGlobalComponent(context, CE_ENGINE_DISPLAY_COMPONENT)->m_inverted = inverted;
#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setInverted(inverted);
#endif
}

void CE_Display_SetFlipX(INOUT CE_ECS_Context* context, IN bool flipX)
{
    CE_Debug("Setting display flipX to: %s", flipX ? "true" : "false");
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_DISPLAY_COMPONENT, displayComp);
    displayComp->m_flipX = flipX;
#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setFlipped(flipX, displayComp->m_flipY);
#endif
}

void CE_Display_SetFlipY(INOUT CE_ECS_Context* context, IN bool flipY)
{
    CE_Debug("Setting display flipY to: %s", flipY ? "true" : "false");
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_DISPLAY_COMPONENT, displayComp);
    displayComp->m_flipY = flipY;
#ifdef CE_BACKEND_PLAYDATE
    PlaydateAPI* pd = CE_GetPlaydateAPI();
    pd->display->setFlipped(displayComp->m_flipX, flipY);
#endif
}

void CE_Display_Reset(INOUT CE_ECS_Context* context)
{
    CE_Debug("Resetting display to default settings");
    CE_Display_SetInverted(context, false);
    CE_Display_SetFlipX(context, false);
    CE_Display_SetFlipY(context, false);
    CE_Display_SetRefreshRate(context, CE_ENGINE_REFRESH_RATE_DEFAULT);
}
