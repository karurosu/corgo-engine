//
//  ecs/core/entity.h
//  Live Entity manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ENTITY_H
#define CORGO_ECS_CORE_ENTITY_H

#include "ecs/types.h"

// Live Entity manipulation functions
bool CE_Entity_IsValid(INOUT CE_ECS_Context* context, IN CE_Id entity);
size_t CE_Entity_GetComponentCount(INOUT CE_ECS_Context* context, IN CE_Id entity);
size_t CE_Entity_GetRelationshipCount(INOUT CE_ECS_Context* context, IN CE_Id entity);

#endif // CORGO_ECS_CORE_ENTITY_H