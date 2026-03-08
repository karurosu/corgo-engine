//
//  engine/sample_scenes/text_scroller.c
//  A simple demo with scrolling text.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"

// Move to a component
static CE_TransformComponent* transformComponent = NULL;
static CE_TransformComponent* transformComponent2 = NULL;

CE_DECLARE_SCENE_CREATE_FUNCTION(TextScroller)
{
    uint16_t x = 0;
    uint16_t y = 0;
        
    CE_TextLabelComponent* textLabelComponent = NULL; // Gonna reuse this for both entities

    CE_Id entityId = CE_INVALID_ID;
    CE_Id entityId2 = CE_INVALID_ID;
    
    if (dataComponent == NULL) {
        CE_Error("No scene data component provided for TextScroller scene.");
        return CE_ERROR;
    } 

    CE_TextScrollerSceneData* sceneData = (CE_TextScrollerSceneData*)dataComponent;
    sceneData->m_xSpeed = 1;
    sceneData->m_ySpeed = 1;

    if (CE_ECS_CreateEntity(context, &entityId, errorCode) != CE_OK) {
        CE_Error("Failed to create demo entity: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context, entityId, CE_TRANSFORM_COMPONENT, NULL, (void**)&transformComponent, errorCode) != CE_OK) {
        CE_Error("Failed to add TransformComponent to demo entity: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context , entityId, CE_TEXT_LABEL_COMPONENT, NULL, (void**)&textLabelComponent, errorCode) != CE_OK) {
        CE_Error("Failed to add TextLabelComponent to demo entity: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    // Set text and font
    if (CE_TextLabelComponent_setText(context, textLabelComponent, transformComponent, "Hello, Corgo Engine!") != CE_OK) {
        CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }
        
    if (CE_TextLabelComponent_setFont(context, textLabelComponent, transformComponent, "/System/Fonts/Roobert-10-Bold.pft") != CE_OK) {
        CE_Error("Failed to set font for TextLabelComponent: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

        // Add to graph
    if (CE_Scene_AddChild(context, CE_Scene_GetRootId(context), entityId, false, errorCode) != CE_OK) {
        CE_Error("Failed to add demo entity to scene graph: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    CE_Debug("Text bounds: %d x %d", transformComponent->m_width, transformComponent->m_height);
    CE_TransformComponent_setPosition(context, transformComponent, (CE_GetDisplayWidth(context)-transformComponent->m_width)/2, (CE_GetDisplayHeight(context)-transformComponent->m_height)/2);

    if (CE_ECS_CreateEntity(context, &entityId2, errorCode) != CE_OK) {
        CE_Error("Failed to create demo entity 2: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context, entityId2, CE_TRANSFORM_COMPONENT, NULL, (void**)&transformComponent2, errorCode) != CE_OK) {
        CE_Error("Failed to add TransformComponent to demo entity 2: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context, entityId2, CE_TEXT_LABEL_COMPONENT, NULL, (void**)&textLabelComponent, errorCode) != CE_OK) {
        CE_Error("Failed to add TextLabelComponent to demo entity 2: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    // Set text and font
    if (CE_TextLabelComponent_setText(context, textLabelComponent, transformComponent2, "WOOF!!") != CE_OK) {
        CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_TextLabelComponent_setFont(context, textLabelComponent, transformComponent2, "/System/Fonts/Asheville-Sans-14-Bold.pft") != CE_OK) {
        CE_Error("Failed to set font for TextLabelComponent 2: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }
    
    // Add to graph
    if (CE_Scene_AddChild(context, entityId, entityId2, false, errorCode) != CE_OK) {
        CE_Error("Failed to add demo entity 2 to scene graph: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    CE_TransformComponent_setZIndex(context, transformComponent2, 1); // Set above the first text label

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(TextScroller)
{
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
