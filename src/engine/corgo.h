//
//  engine/corgo.h
//  Primary Engine include, includes both ECS and Engine.
//  This is the public API for Corgo Engine, most C files can include this.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ENGINE_CORGO_H
#define CORGO_ENGINE_CORGO_H

#include "config.h"
#include "core/platform.h"
#include "core/memory.h"
#include "ecs/ecs.h"
#include "scenes.h"
#include "input_maps.h"
#include "input_actions.h"

// Include debug tools
#ifdef CE_DEBUG_BUILD
#include "core/debug.h"
#endif // CE_DEBUG_BUILD

#endif // CORGO_ENGINE_CORGO_H
