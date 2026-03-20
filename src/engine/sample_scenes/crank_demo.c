//
//  engine/sample_scenes/crank_demo.c
//  A simple demo scene to showcase crank dock state and angle.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/scene.h"

static CE_TextLabelComponent* crankStateText = NULL;
static CE_TransformComponent* crankStateTransform = NULL;

static CE_TextLabelComponent* crankAngleText = NULL;
static CE_TransformComponent* crankAngleTransform = NULL;

static bool hasLastDockState = false;
static bool lastDocked = true;
static int lastAngleRounded = -1;

CE_DECLARE_SCENE_CREATE_FUNCTION(CrankDemo)
{
    hasLastDockState = false;
    lastDocked = true;
    lastAngleRounded = -1; 

    {
        CES_CREATE_ENTITY(crank_state_label);
        CES_ADD_COMPONENT_EPTR(crank_state_label, CE_TRANSFORM_COMPONENT, crankStateTransform);
        CES_ADD_COMPONENT_EPTR(crank_state_label, CE_TEXT_LABEL_COMPONENT, crankStateText);

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setStaticText(context, crankStateText, crankStateTransform, "Crank State: docked"),
            "Failed to set crank state text");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, crankStateText, crankStateTransform, "/System/Fonts/Roobert-10-Bold.pft"),
            "Failed to set crank state font");

        CES_CHECK_RESULT(
            CE_TransformComponent_setPosition(context, crankStateTransform, (CE_GetDisplayWidth(context) - crankStateTransform->m_width) / 2, 90),
            "Failed to set crank state label position");

        CES_ADD_CHILD(CES_ROOT_ENTITY, crank_state_label);
    }

    {
        CES_CREATE_ENTITY(crank_angle_label);
        CES_ADD_COMPONENT_EPTR(crank_angle_label, CE_TRANSFORM_COMPONENT, crankAngleTransform);
        CES_ADD_COMPONENT_EPTR(crank_angle_label, CE_TEXT_LABEL_COMPONENT, crankAngleText);

        CES_BUILD_STRING(crankAngleText->m_text, "Crank Angle: 0");
        
        CES_CHECK_RESULT(
            CE_TextLabelComponent_update(context, crankAngleText, crankAngleTransform),
            "Failed to set crank angle text");

        CES_CHECK_RESULT(
            CE_TextLabelComponent_setFont(context, crankAngleText, crankAngleTransform, "/System/Fonts/Roobert-10-Bold.pft"),
            "Failed to set crank angle font");

        CES_CHECK_RESULT(
            CE_TransformComponent_setPosition(context, crankAngleTransform, (CE_GetDisplayWidth(context) - crankAngleTransform->m_width) / 2, 120),
            "Failed to set crank angle label position");

        CES_ADD_CHILD(CES_ROOT_ENTITY, crank_angle_label);
    }

    // Enable reading crank values
    CES_ENABLE_INPUT_FEATURE(CE_Input_Feature_Crank);

    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(CrankDemo)
{
    CES_INITIALIZE_INPUT();

    const bool isDocked = CES_IS_CRANK_DOCKED();
    const int angleRounded = CE_GET_CRANK_ANGLE_ROUNDED();

    if (!hasLastDockState || (lastDocked != isDocked)) {
        const char* stateText = isDocked ? "Crank State: docked" : "Crank State: undocked";
        CES_CHECK_RESULT(
            CE_TextLabelComponent_setStaticText(context, crankStateText, crankStateTransform, stateText),
            "Failed to set crank state text");

        lastDocked = isDocked;
        hasLastDockState = true;
    }

    if (lastAngleRounded != angleRounded) {
        CES_BUILD_STRING(crankAngleText->m_text, "Crank Angle: ", angleRounded);
        
        CES_CHECK_RESULT(
            CE_TextLabelComponent_update(context, crankAngleText, crankAngleTransform),
            "Failed to set crank angle text");

        lastAngleRounded = angleRounded;
    }
    return CE_OK;
}

#undef CES_RAW_INPUT_IS_ACTIVE

CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(CrankDemo)
{
    scene->m_id = "CrankDemo";
    scene->m_createFunction = CE_SCENE_CREATE_FUNCTION(CrankDemo);
    scene->m_runFunction = CE_SCENE_RUN_FUNCTION(CrankDemo);
    scene->m_scriptDataComponentType = CE_INVALID_TYPE_ID;
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
