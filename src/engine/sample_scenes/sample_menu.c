//
//  engine/sample_scenes/sample_menu.c
//  A menu to select wwhich sample to load.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

// Doing this early to avoid including all engine headers when sample scenes are not included
#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/corgo.h"
#include "engine/shortcuts/scene.h"

// Move to a component
static int demo = -1;

CE_DECLARE_SCENE_CREATE_FUNCTION(SampleMenu)
{
    return CE_OK;
}

CE_DECLARE_SCENE_RUN_FUNCTION(SampleMenu)
{
    if (demo == -1)
    {
        // Load first scene immediately
        CES_LOAD_SCENE(TextScroller);
        demo = 0;
        return CE_OK;
    }

    demo = (demo + 1) % 2;
    if (demo == 0) {
        CES_LOAD_SCENE(TextScroller);
    } else {
        CES_LOAD_SCENE(TextScroller2);
    }

    return CE_OK;
}

CE_DECLARE_SCENE_LOAD_DATA_FUNCTION(SampleMenu)
{
    // Populate the scene data with the scene id and function pointers
    scene->m_id = "SampleMenu";
    scene->m_createFunction = CE_SCENE_CREATE_FUNCTION(SampleMenu);
    scene->m_runFunction = CE_SCENE_RUN_FUNCTION(SampleMenu);
    scene->m_scriptDataComponentType = CE_INVALID_TYPE_ID;
    return CE_OK;
}

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
