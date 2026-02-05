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

// Include component headers
#include "components/text_label.h"
#include "components/transform.h"
#include "components/sprite.h"

// Global components
#include "components/scene_graph.h"

#define CE_COMPONENT_DESC_ENGINE(X) \
	X(CE_TRANSFORM_COMPONENT, 10, CE_TransformComponent, CE_DEFAULT_COMPONENT_CAPACITY)\
	X(CE_SPRITE_COMPONENT, 11, CE_SpriteComponent, 32)\
	X(CE_TEXT_LABEL_COMPONENT, 12, CE_TextLabelComponent, 16)

#define CE_GLOBAL_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_ASSET_CACHE, CE_Engine_AssetCache_Component)\
	X(CE_ENGINE_SCENE_GRAPH_COMPONENT, CE_SceneGraphComponent)

#endif // CORGO_ENGINE_COMPONENTS_H