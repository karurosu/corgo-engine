//
//  engine/components/image.h
//  Image component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_IMAGE_H
#define CORGO_ENGINE_COMPONENTS_IMAGE_H

#include "ecs/types.h"
#include "engine/assets.h"


//// Image Component
typedef struct CEImageComponent {
	CE_ASSET_PTR(CE_ASSET_TYPE_BITMAP) m_imagePtr;
} CE_ImageComponent;

typedef struct CE_TransformComponent CE_TransformComponent;

/********************
 * Helper functions
 ********************/

CE_Result CE_ImageComponent_setImage(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, IN CE_TransformComponent *transform, IN const char* imageName);
CE_Result CE_ImageComponent_clearImage(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component,  IN CE_TransformComponent *transform);
CE_Result CE_ImageComponent_getImageBounds(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, OUT uint16_t* width, OUT uint16_t* height);
CE_Result CE_ImageComponent_update(INOUT CE_ECS_Context* context, INOUT CE_ImageComponent* component, IN CE_TransformComponent *transform);

#endif // CORGO_ENGINE_COMPONENTS_IMAGE_H