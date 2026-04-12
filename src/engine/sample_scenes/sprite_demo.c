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

CE_DECLARE_SCENE_CREATE_FUNCTION(SpriteDemo)
{
    CES_CREATE_ENTITY(corgi_sprite);
    CE_ImageComponent* imageComponent = NULL;
    
    CES_ADD_COMPONENT_EPTR(corgi_sprite, CE_IMAGE_COMPONENT, imageComponent);
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

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(SpriteDemo)
{
    CES_INITIALIZE_INPUT();

    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonUp)) {
        corgiTransformComponent->m_y -= 1;
        CE_Engine_SceneGraph_MarkDirty(context);
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonDown)) {
        corgiTransformComponent->m_y += 1;
        CE_Engine_SceneGraph_MarkDirty(context);
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonLeft)) {
        corgiTransformComponent->m_x -= 1;
        CE_Engine_SceneGraph_MarkDirty(context);
    }
    if (CES_INPUT_IS_PRESSED(CE_Input_ButtonRight)) {
        corgiTransformComponent->m_x += 1;
        CE_Engine_SceneGraph_MarkDirty(context);
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