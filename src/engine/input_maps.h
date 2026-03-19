//
//  engine/input_maps.h
//  Main include for engine input maps.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_INPUT_MAPS_H
#define CORGO_ENGINE_INPUT_MAPS_H

#include "engine/config.h"
#ifdef CE_ENGINE_INCLUDE_SAMPLE_SCENES

#include "engine/components/input.h"

// The following are built-in demo input maps used to test and demonstrate engine features.
CE_DECLARE_ACTION_MAP_FUNCTION(SampleInputMap1);
CE_DECLARE_ACTION_MAP_FUNCTION(SampleInputMap2);

#endif // CE_ENGINE_INCLUDE_SAMPLE_SCENES
#endif // CORGO_ENGINE_INPUT_MAPS_H
