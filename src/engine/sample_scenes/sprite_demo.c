//
//  engine/sample_scenes/sprite_demo.c
//  A simple demo scene to showcase sprite rendering.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/scene.h"

static CE_TransformComponent* corgiTransformComponent = NULL;
static CE_TransformComponent* catTransformComponent = NULL;
static CE_TextLabelComponent* labelTextComponent = NULL;
static CE_TransformComponent* labelTransformComponent = NULL;
static int mode = 0;

CE_DECLARE_SCENE_CREATE_FUNCTION(SpriteDemo)
{
    {
        CES_CREATE_ENTITY(corgi_sprite);
        
        CES_ADD_COMPONENT_PTR(corgi_sprite, CE_IMAGE_COMPONENT, imageComponent);
        CES_ADD_COMPONENT_EPTR(corgi_sprite, CE_TRANSFORM_COMPONENT, corgiTransformComponent);

        CES_CHECK_RESULT(
            CE_ImageComponent_setImage(context, imageComponent, corgiTransformComponent, "/demos/images/corny.png"),
            "Failed to set corgi sprite image");
        
        CES_CHECK_RESULT(
            CE_TransformComponent_setPosition(context, corgiTransformComponent, 
                (CE_GetDisplayWidth(context) - corgiTransformComponent->m_width) / 2, 
                (CE_GetDisplayHeight(context) - corgiTransformComponent->m_height) / 2),
            "Failed to set corgi sprite position");
        
        CES_ADD_TO_ROOT(corgi_sprite);
    }

    {
        CES_CREATE_ENTITY(cat_sprite);
        
        CES_ADD_COMPONENT_PTR(cat_sprite, CE_IMAGE_COMPONENT, imageComponent);
        CES_ADD_COMPONENT_EPTR(cat_sprite, CE_TRANSFORM_COMPONENT, catTransformComponent);

        CES_CHECK_RESULT(
            CE_ImageComponent_setImage(context, imageComponent, catTransformComponent, "/demos/images/belly.png"),
            "Failed to set cat sprite image");
        
        CES_CHECK_RESULT(
            CE_TransformComponent_setPosition(context, catTransformComponent, 
                (CE_GetDisplayWidth(context) - catTransformComponent->m_width) / 4, 
                (CE_GetDisplayHeight(context) - catTransformComponent->m_height) / 4),
            "Failed to set cat sprite position");
        
        CES_ADD_TO_ROOT(cat_sprite);
    }

    {
        CES_CREATE_ENTITY(label);

        CES_ADD_COMPONENT_EPTR(label, CE_TEXT_LABEL_COMPONENT, labelTextComponent);
        CES_ADD_COMPONENT_EPTR(label, CE_TRANSFORM_COMPONENT, labelTransformComponent);

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setStaticText(context, labelTextComponent, labelTransformComponent, "(A) to switch. Now moving: Corny"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
             CE_TextLabelComponent_setFont(context, labelTextComponent, labelTransformComponent, "/System/Fonts/Roobert-10-Bold.pft"),
            "Failed to set font for TextLabelComponent");
        
        CES_CHECK_RESULT(
            CE_TransformComponent_setPosition(context, labelTransformComponent, 5, CE_GetDisplayHeight(context) - labelTransformComponent->m_height),
            "Failed to set position for TransformComponent");
        
        CE_TransformComponent_setFlags(labelTransformComponent, CE_TransformComponent_Flags_FixedPosition); // Make the label have a fixed position that doesn't move with the scene graph

        CES_ADD_TO_ROOT(label);

    }

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(SpriteDemo)
{
    CES_INITIALIZE_INPUT();

    if (CES_INPUT_WAS_JUST_PRESSED(CE_Input_ButtonA)) {
        mode = (mode + 1) % 3; // Cycle through modes
        switch(mode) {
            case 0:
                CE_TextLabelComponent_setStaticText(context, labelTextComponent, labelTransformComponent, "(A) to switch. Now moving: Corny");
                break;
            case 1:
                CE_TextLabelComponent_setStaticText(context, labelTextComponent, labelTransformComponent, "(A) to switch. Now moving: Belly");
                break;
            case 2:
                CE_TextLabelComponent_setStaticText(context, labelTextComponent, labelTransformComponent, "(A) to switch. Now moving: Camera");
                break;
        }
    }

    uint16_t x = 0;
    uint16_t y = 0;

    switch(mode) {
        case 0: // Corgi mode
            x = corgiTransformComponent->m_x;
            y = corgiTransformComponent->m_y;
            break;
        case 1: // Cat mode
            x = catTransformComponent->m_x;
            y = catTransformComponent->m_y;
            break;
        case 2: // Camera mode
            x = CE_Engine_Camera_GetX(context);
            y = CE_Engine_Camera_GetY(context);
            break;
    }
    

    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonUp)) {
        y -= 1;
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonDown)) {
        y += 1;
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonLeft)) {
        x -= 1;
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonRight)) {
        x += 1;
    }
    
    switch(mode) {
        case 0: // Corgi mode
            CE_TransformComponent_setPosition(context, corgiTransformComponent, x, y);
            break;
        case 1: // Cat mode
            CE_TransformComponent_setPosition(context, catTransformComponent, x, y);
            break;
        case 2: // Camera mode
            CE_Engine_Camera_SetPosition(context, x, y);
            break;
    }


    return CE_OK;
}

CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(SpriteDemo)
{
    // Populate the scene data with the scene id and function pointers
    scene->m_id = "SpriteDemo";
    scene->m_createFunction = CE_SCENE_CREATE_FUNCTION(SpriteDemo);
    scene->m_runFunction = CE_SCENE_RUN_FUNCTION(SpriteDemo);
    scene->m_scriptDataComponentType = CE_INVALID_TYPE_ID;
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES