//
//  engine/components.h
//  List of engine ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_H
#define CORGO_ENGINE_COMPONENTS_H

#include "../ecs/core/component.h"
#include "core/platform.h"
#include "engine/assets.h"

//// Transform Component
typedef struct CETransformComponent {
    uint32_t x;
	uint32_t y;
	uint32_t z; // Z-index for layering
} CE_TransformComponent;

//// Sprite Component
typedef struct CESpriteComponent {
	bool visible;
} CE_SpriteComponent;

//// Text Label Component
typedef struct CETextLabelComponent {
	char text[256];
	const char *fontName;
	CE_ASSET_PTR(CE_ASSET_TYPE_FONT) fontPtr;
} CE_TextLabelComponent;

#define CE_COMPONENT_DESC_ENGINE(X) \
	X(CE_TRANSFORM_COMPONENT, 10, CE_TransformComponent, CE_DEFAULT_COMPONENT_CAPACITY)\
	X(CE_SPRITE_COMPONENT, 11, CE_SpriteComponent, 32)\
	X(CE_TEXT_LABEL_COMPONENT, 12, CE_TextLabelComponent, 16)

#define CE_GLOBAL_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_ASSET_CACHE, CE_Engine_AssetCache_Component)

#endif // CORGO_ENGINE_COMPONENTS_H