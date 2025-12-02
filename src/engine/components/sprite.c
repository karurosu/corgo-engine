//
//  engine/components/sprite.c
//  A wrapper around Playdate's sprites
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "ecs/components.h"

void CE_SPRITE_COMPONENT_init(OUT CE_SpriteComponent* component)
{
    component->visible = true;
}

void CE_SPRITE_COMPONENT_cleanup(OUT CE_SpriteComponent* component)
{
    // No dynamic resources to clean up for CE_SpriteComponent
}

CE_GENERATE_COMPONENT_IMP(CE_SPRITE_COMPONENT, 11, CE_SpriteComponent)