//
//  engine/components/sprite.c
//  A wrapper around Playdate's sprites
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/ecs.h"

CE_DEFINE_COMPONENT_INIT(CE_SPRITE_COMPONENT)
{
    component->visible = true;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_SPRITE_COMPONENT)
{
    // No dynamic resources to clean up for CE_SpriteComponent
    return CE_OK;
}

CE_GENERATE_COMPONENT_IMP(CE_SPRITE_COMPONENT)
