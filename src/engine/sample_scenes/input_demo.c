//
//  engine/sample_scenes/input_demo.c
//  A simple demo to showcase input handling.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/scene.h"

// Move to a component
static CE_TextLabelComponent* pressed_text = NULL;
static CE_TextLabelComponent* released_text = NULL;
static CE_TextLabelComponent* held_text = NULL;
static CE_TextLabelComponent* held2_text = NULL;
static CE_TextLabelComponent* mapping_text = NULL;

static CE_TransformComponent* pressed_transform = NULL;
static CE_TransformComponent* released_transform = NULL;
static CE_TransformComponent* held_transform = NULL;
static CE_TransformComponent* held2_transform = NULL;
static CE_TransformComponent* mapping_transform = NULL;

static float timer = 0.0f;
#define TIMER_END 5.0f

static int currentMapping = 0;
static bool held = false;
static bool held2 = false;

CE_DECLARE_SCENE_CREATE_FUNCTION(InputDemo)
{
    timer = 0.0f;
    currentMapping = 0;

    {
        CES_CREATE_ENTITY(pressed_label);
        CES_ADD_COMPONENT_EPTR(pressed_label, CE_TRANSFORM_COMPONENT, pressed_transform);
        CES_ADD_COMPONENT_EPTR(pressed_label, CE_TEXT_LABEL_COMPONENT, pressed_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, pressed_text, pressed_transform, "Pressed Event: Not Triggered"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, pressed_text, pressed_transform, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, pressed_transform, (CE_GetDisplayWidth(context)-pressed_transform->m_width)/2, 40),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, pressed_label);
    }

    {
        CES_CREATE_ENTITY(released_label);
        CES_ADD_COMPONENT_EPTR(released_label, CE_TRANSFORM_COMPONENT, released_transform);
        CES_ADD_COMPONENT_EPTR(released_label, CE_TEXT_LABEL_COMPONENT, released_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, released_text, released_transform, "Released Event: Not Triggered"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, released_text, released_transform, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, released_transform, (CE_GetDisplayWidth(context)-released_transform->m_width)/2, 70),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, released_label);
    }

    {
        CES_CREATE_ENTITY(held_label);
        CES_ADD_COMPONENT_EPTR(held_label, CE_TRANSFORM_COMPONENT, held_transform);
        CES_ADD_COMPONENT_EPTR(held_label, CE_TEXT_LABEL_COMPONENT, held_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, held_text, held_transform, "Held Event: Not Triggered"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, held_text, held_transform, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, held_transform, (CE_GetDisplayWidth(context)-held_transform->m_width)/2, 100),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, held_label);
    }

    {
        CES_CREATE_ENTITY(held2_label);
        CES_ADD_COMPONENT_EPTR(held2_label, CE_TRANSFORM_COMPONENT, held2_transform);
        CES_ADD_COMPONENT_EPTR(held2_label, CE_TEXT_LABEL_COMPONENT, held2_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, held2_text, held2_transform, "Double Held Event: Not Triggered"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, held2_text, held2_transform, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, held2_transform, (CE_GetDisplayWidth(context)-held2_transform->m_width)/2, 130),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, held2_label);
    }
    
    {
        CES_CREATE_ENTITY(mapping_label);
        CES_ADD_COMPONENT_EPTR(mapping_label, CE_TRANSFORM_COMPONENT, mapping_transform);
        CES_ADD_COMPONENT_EPTR(mapping_label, CE_TEXT_LABEL_COMPONENT, mapping_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, mapping_text, mapping_transform, "Current Mapping: None"), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, mapping_text, mapping_transform, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, mapping_transform, (CE_GetDisplayWidth(context)-mapping_transform->m_width)/2, 160),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, mapping_label);
    }

    {
        CES_CREATE_ENTITY(instructions_label);
        CES_ADD_COMPONENT_PTR(instructions_label, CE_TRANSFORM_COMPONENT, transformComponent);
        CES_ADD_COMPONENT_PTR(instructions_label, CE_TEXT_LABEL_COMPONENT, instructions_text);
        
        // Set text and font
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setText(context, instructions_text, transformComponent, "Use Up/Down to change mapping. Use A/B to test input.\nValue and 'None' mapping reset after 5 seconds."), 
            "Failed to set text for TextLabelComponent");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, instructions_text, transformComponent, "/System/Fonts/Roobert-10-Bold.pft"), 
            "Failed to set font for TextLabelComponent");

        CES_CHECK_RESULT(
        CE_TransformComponent_setPosition(context, transformComponent, (CE_GetDisplayWidth(context)-transformComponent->m_width)/2, 200),
        "Failed to set position for TransformComponent");
        
        // Add to root of graph
        CES_ADD_CHILD(CES_ROOT_ENTITY, instructions_label);
    }

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(InputDemo)
{
    CES_INITIALIZE_INPUT();

    timer += deltaTime;

    if (timer >= TIMER_END) {
        CE_TextLabelComponent_setText(context, pressed_text, pressed_transform, "Pressed Event: Not Triggered");
        CE_TextLabelComponent_setText(context, released_text, released_transform, "Released Event: Not Triggered");
        timer = 0.0f;

        if (currentMapping == 0)
        {
            CE_TextLabelComponent_setText(context, mapping_text, mapping_transform, "Current Mapping: A");
            CES_PUSH_ACTION_MAP(SampleInputMap1);
            currentMapping = 1;
        }
    }

    if(CES_IA_ISACTIVE(CE_IA_DEMO_PRESS)) {
        CE_TextLabelComponent_setText(context, pressed_text, pressed_transform, "Pressed Event: Triggered");
        timer = 0.0f;
    }
    
    if(CES_IA_ISACTIVE(CE_IA_DEMO_RELEASE)) {
        CE_TextLabelComponent_setText(context, released_text, released_transform, "Released Event: Triggered");
        timer = 0.0f;
    }

    if (held && !CES_IA_ISACTIVE(CE_IA_DEMO_HELD)) {
        held = false;
        CE_TextLabelComponent_setText(context, held_text, held_transform, "Held Event: Not Triggered");
    } else if (!held && CES_IA_ISACTIVE(CE_IA_DEMO_HELD)) {
        held = true;
        CE_TextLabelComponent_setText(context, held_text, held_transform, "Held Event: Triggered");
    }

    if (held2 && !CES_IA_ISACTIVE(CE_IA_DEMO_HELD_2)) {
        held2 = false;
        CE_TextLabelComponent_setText(context, held2_text, held2_transform, "Double Held Event: Not Triggered");
    } else if (!held2 && CES_IA_ISACTIVE(CE_IA_DEMO_HELD_2)) {
        held2 = true;
        CE_TextLabelComponent_setText(context, held2_text, held2_transform, "Double Held Event: Triggered");
    }

    if (CES_IA_ISACTIVE(CE_IA_NEXT_INPUT_MAP)) {
        if (currentMapping == 1) {
            CE_TextLabelComponent_setText(context, mapping_text, mapping_transform, "Current Mapping: B");
            CES_PUSH_ACTION_MAP(SampleInputMap2);
            currentMapping = 2;
            timer = TIMER_END;
        }
        else{
            // Should never reach here since the input map should never trigger
            CE_Error("Invalid state: Next input map action triggered but current mapping is %d", currentMapping);
            return CE_ERROR;
        }
    }

    if (CES_IA_ISACTIVE(CE_IA_PREVIOUS_INPUT_MAP)) {
        if (currentMapping == 1) {
            CE_TextLabelComponent_setText(context, mapping_text, mapping_transform, "Current Mapping: None");
            CES_POP_ACTION_MAP();
            currentMapping = 0;
            timer = 0.0f;
        } else if (currentMapping == 2) {
            CE_TextLabelComponent_setText(context, mapping_text, mapping_transform, "Current Mapping: A");
            CES_POP_ACTION_MAP();
            currentMapping = 1;
            timer = TIMER_END;
        }
    }

    return CE_OK;
}

CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(InputDemo)
{
    // Populate the scene data with the scene id and function pointers
    scene->m_id = "InputDemo";
    scene->m_createFunction = CE_SCENE_CREATE_FUNCTION(InputDemo);
    scene->m_runFunction = CE_SCENE_RUN_FUNCTION(InputDemo);
    scene->m_scriptDataComponentType = CE_INVALID_TYPE_ID;
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
