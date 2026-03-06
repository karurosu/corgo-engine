//
//  engine/scenes/text_scroller.c
//  A simple demo with scrolling text.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "ecs/ecs.h"

// Move to a component
static uint16_t x_speed = 1;
static uint16_t y_speed = 1;
static CE_TransformComponent* transformComponent = NULL;
static CE_TransformComponent* transformComponent2 = NULL;

CE_DECLARE_SCENE_CREATE_FUNCTION(TextScroller)
{
    uint16_t x = 0;
    uint16_t y = 0;
        
    CE_TextLabelComponent* textLabelComponent = NULL; // Gonna reuse this for both entities

    CE_Id _compId = CE_INVALID_ID; // Using this as a temp variable since we don't care about the ID

    CE_Id entityId = CE_INVALID_ID;
    CE_Id entityId2 = CE_INVALID_ID;
    
    if (CE_ECS_CreateEntity(context, &entityId, errorCode) != CE_OK) {
        CE_Error("Failed to create demo entity: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context, entityId, CE_TRANSFORM_COMPONENT, &_compId, (void**)&transformComponent, errorCode) != CE_OK) {
        CE_Error("Failed to add TransformComponent to demo entity: %s", CE_GetErrorMessage(*errorCode));
        return CE_ERROR;
    }

    if (CE_Entity_AddComponent(context , entityId, CE_TEXT_LABEL_COMPONENT, &_compId, (void**)&textLabelComponent, errorCode) != CE_OK) {
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

    if (CE_Entity_AddComponent(context, entityId2, CE_TRANSFORM_COMPONENT, &_compId, (void**)&transformComponent2, errorCode) != CE_OK) {
        CE_Error("Failed to add TransformComponent to demo entity 2: %s", CE_GetErrorMessage(*errorCode));
        return -1;
    }

    if (CE_Entity_AddComponent(context, entityId2, CE_TEXT_LABEL_COMPONENT, &_compId, (void**)&textLabelComponent, errorCode) != CE_OK) {
        CE_Error("Failed to add TextLabelComponent to demo entity 2: %s", CE_GetErrorMessage(*errorCode));
        return -1;
    }

    // Set text and font
    if (CE_TextLabelComponent_setText(context, textLabelComponent, transformComponent2, "WOOF!!") != CE_OK) {
        CE_Error("Failed to set text for TextLabelComponent: %s", CE_GetErrorMessage(*errorCode));
        return -1;
    }

    if (CE_TextLabelComponent_setFont(context, textLabelComponent, transformComponent2, "/System/Fonts/Asheville-Sans-14-Bold.pft") != CE_OK) {
        CE_Error("Failed to set font for TextLabelComponent 2: %s", CE_GetErrorMessage(*errorCode));
        return -1;
    }
    
    // Add to graph
    if (CE_Scene_AddChild(context, entityId, entityId2, false, errorCode) != CE_OK) {
        CE_Error("Failed to add demo entity 2 to scene graph: %s", CE_GetErrorMessage(*errorCode));
        return -1;
    }

    CE_TransformComponent_setZIndex(context, transformComponent2, 1); // Set above the first text label

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(TextScroller)
{
    const uint16_t x = transformComponent->m_x + x_speed;
    const uint16_t y = transformComponent->m_y + y_speed;

    if (x <= 0 || x + transformComponent->m_width >= CE_GetDisplayWidth(context)) {
        x_speed = -x_speed;
    }
    if (y <= 0 || y + transformComponent->m_height >= CE_GetDisplayHeight(context)) {
        y_speed = -y_speed;
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
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
