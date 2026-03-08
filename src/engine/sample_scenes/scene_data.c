//
//  engine/scenes/scene_data.h
//  Data components for engine demo scripts
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "engine/corgo.h"

CE_DEFINE_COMPONENT_INIT(CE_TEXT_SCROLLER_SCENE_DATA_COMPONENT)
{
    component->m_xSpeed = 0;
    component->m_ySpeed = 0;

    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_TEXT_SCROLLER_SCENE_DATA_COMPONENT)
{
    return CE_OK;
}
