//
//  engine/components.h
//  List of engine ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_H
#define CORGO_ENGINE_COMPONENTS_H

#include "../ecs/core/component.h"

//// Transform Component
typedef struct CETransformComponent {
    uint32_t x;
	uint32_t y;
} CE_TransformComponent;

//// Sprite Component
typedef struct CESpriteComponent {
	bool visible;
} CE_SpriteComponent;

#define CE_COMPONENT_DESC_ENGINE(X) \
	X(CE_TRANSFORM_COMPONENT, 10, CE_TransformComponent)      \
	X(CE_SPRITE_COMPONENT, 11, CE_SpriteComponent)

#endif // CORGO_ENGINE_COMPONENTS_H