//
//  engine/components.h
//  List of engine ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_H
#define CORGO_ENGINE_COMPONENTS_H

#include "../ecs/core/component.h"
#include "core/platform.h"

// Global components
#include "components/scene_graph.h"
#include "core/display.h"
#include "engine/assets.h"
#include "components/camera.h"

// Include component headers
#include "components/text_label.h"
#include "components/transform.h"
#include "components/sprite.h"

#define CE_COMPONENT_DESC_ENGINE(X) \
	X(CE_TRANSFORM_COMPONENT, 10, CE_TransformComponent, CE_DEFAULT_COMPONENT_CAPACITY)\
	X(CE_SPRITE_COMPONENT, 11, CE_SpriteComponent, 32)\
	X(CE_TEXT_LABEL_COMPONENT, 12, CE_TextLabelComponent, 16)\


#define CE_GLOBAL_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_ASSET_CACHE, CE_Engine_AssetCacheComponent)\
	X(CE_ENGINE_SCENE_GRAPH_COMPONENT, CE_SceneGraphComponent)\
	X(CE_ENGINE_DISPLAY_COMPONENT, CE_DisplayComponent)\
	X(CE_ENGINE_CAMERA_COMPONENT, CE_CameraComponent)\


#endif // CORGO_ENGINE_COMPONENTS_H