//
//  engine/scenes.h
//  Main include for engine demo scenes.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_SCENES_H
#define CORGO_ENGINE_SCENES_H

#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/core/scene.h"

// The following are built-in demo scenes used to test and demonstrate engine features.
CE_DECLARE_SCENE(SampleMenu)
CE_DECLARE_SCENE(TextScroller)
CE_DECLARE_SCENE(TextScroller2)
CE_DECLARE_SCENE(InputDemo)

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
#endif // CORGO_ENGINE_SCENES_H
