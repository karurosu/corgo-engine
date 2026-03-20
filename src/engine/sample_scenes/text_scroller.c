//
//  engine/sample_scenes/text_scroller.c
//  A simple demo with scrolling text.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/scene.h"

// Move to a component
static CE_TransformComponent* transformComponent = NULL;
static CE_TransformComponent* transformComponent2 = NULL;
static float timer = 0.0f;

CE_DECLARE_SCENE_CREATE_FUNCTION(TextScroller)
{
    if (dataComponent == NULL) {
        CE_Error("No scene data component provided for TextScroller scene.");
        return CE_ERROR;
    } 

    CE_TextScrollerSceneData* sceneData = (CE_TextScrollerSceneData*)dataComponent;
    sceneData->m_xSpeed = 1;
    sceneData->m_ySpeed = 1;

    timer = 0.0f;

    CES_CREATE_ENTITY(parent);
    CES_ADD_COMPONENT_EPTR(parent, CE_TRANSFORM_COMPONENT, transformComponent);
    CES_ADD_COMPONENT_PTR(parent, CE_TEXT_LABEL_COMPONENT, textLabelComponent);
    
    // Set text and font
    CES_CHECK_RESULT(
        CE_TextLabelComponent_setStaticText(context, textLabelComponent, transformComponent, "Hello, Corgo Engine!"), 
        "Failed to set text for TextLabelComponent");

    CES_CHECK_RESULT(
         CE_TextLabelComponent_setFont(context, textLabelComponent, transformComponent, "/System/Fonts/Roobert-10-Bold.pft"), 
        "Failed to set font for TextLabelComponent");

    // Add to root of graph
    CES_ADD_CHILD(CES_ROOT_ENTITY, parent);
    
    CE_Debug("Text bounds: %d x %d", transformComponent->m_width, transformComponent->m_height);
    
    CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, transformComponent, (CE_GetDisplayWidth(context)-transformComponent->m_width)/2, (CE_GetDisplayHeight(context)-transformComponent->m_height)/2),
        "Failed to set position for TransformComponent");
    
    CES_CREATE_ENTITY(child);
    CES_ADD_COMPONENT_EPTR(child, CE_TRANSFORM_COMPONENT, transformComponent2);
    CES_ADD_COMPONENT_EPTR(child, CE_TEXT_LABEL_COMPONENT, textLabelComponent); // Reuse *textLabelComponent from the previous entity

    // Set text and font
    CES_CHECK_RESULT(
        CE_TextLabelComponent_setStaticText(context, textLabelComponent, transformComponent2, "WOOF!!"),
        "Failed to set text for TextLabelComponent");

    CES_CHECK_RESULT(
        CE_TextLabelComponent_setFont(context, textLabelComponent, transformComponent2, "/System/Fonts/Asheville-Sans-14-Bold.pft"),
        "Failed to set font for TextLabelComponent 2");
    
    // Add to graph
    CES_ADD_CHILD(parent, child);

    CES_CHECK_RESULT(
        CE_TransformComponent_setZIndex(context, transformComponent2, 1),
        "Failed to set Z index for child entity");

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(TextScroller)
{
    timer += deltaTime;
    if (timer >= 5.0f) 
    {
        CES_LOAD_SCENE(SampleMenu);
        return CE_OK;
    }

    CE_TextScrollerSceneData* sceneData = NULL;
    CE_Entity_FindFirstComponent(context, CE_Scene_GetRootId(context), CE_TEXT_SCROLLER_SCENE_DATA_COMPONENT, NULL, (void**)&sceneData, errorCode);

    const uint16_t x = transformComponent->m_x + sceneData->m_xSpeed;
    const uint16_t y = transformComponent->m_y + sceneData->m_ySpeed;

    if (x <= 0 || x + transformComponent->m_width >= CE_GetDisplayWidth(context)) {
        sceneData->m_xSpeed *= -1;
    }
    if (y <= 0 || y + transformComponent->m_height >= CE_GetDisplayHeight(context)) {
        sceneData->m_ySpeed *= -1;
    }

    // Update transform component position
    CE_TransformComponent_setPosition(context, transformComponent, x, y);

    // Also update the second component to test Z ordering
    CE_TransformComponent_setPosition(context, transformComponent2, (transformComponent2->m_x + 2) % transformComponent->m_width, 0);

    return CE_OK;
}

CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(TextScroller)
{
    // Populate the scene data with the scene id and function pointers
    scene->m_id = "TextScroller";
    scene->m_createFunction = CE_SCENE_CREATE_FUNCTION(TextScroller);
    scene->m_runFunction = CE_SCENE_RUN_FUNCTION(TextScroller);
    scene->m_scriptDataComponentType = CE_TEXT_SCROLLER_SCENE_DATA_COMPONENT;
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
