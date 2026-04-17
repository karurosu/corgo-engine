//
//  engine/components/transform.h
//  Transform component definition.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_COMPONENTS_TRANSFORM_H
#define CORGO_ENGINE_COMPONENTS_TRANSFORM_H

#include "ecs/types.h"

typedef enum  {
    CE_TransformComponent_Flags_None = 0, // No special behavior
    CE_TransformComponent_Flags_Dirty = 1 << 0, // Whether the component is dirty and needs to be updated in the scene graph (not currently used)
    CE_TransformComponent_Flags_InSceneGraph = 1 << 1, // Whether the component is currently in the scene graph
    CE_TransformComponent_Flags_FixedPosition = 1 << 2, // Whether the component has a fixed position and does not inherit from its parent or camera
    CE_TransformComponent_Flags_InheritsZIndex = 1 << 3, // Whether the component inherits Z-index from its parent or camera (not currently used)
    CE_TransformComponent_Flags_YZIndex = 1 << 4, // Whether the Y coordinate should be used as Z-index for layering (isometric sorting) (not currently used)
} CE_TransformComponent_Flags;

//// Transform Component
typedef struct CE_TransformComponent {
    // Local position
    int16_t m_x;
	int16_t m_y;
    // Z-index for layering
	int16_t m_z;
    // Size
    uint16_t m_width;
    uint16_t m_height;
    // Flags
    CE_TransformComponent_Flags m_flags;
} CE_TransformComponent;

/// Public API

/**
 * @brief Set the position (X, Y) of the TransformComponent.
 * This must be called to update the values to guarantee the scene graph is updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param component[in,out] The TransformComponent to update.
 * @param x[in] The new X position.
 * @param y[in] The new Y position.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TransformComponent_setPosition(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t x, IN int16_t y);

/**
 * @brief Set the Z-index of the TransformComponent.
 * This must be called to update the value to guarantee the scene graph is updated accordingly.
 * 
 * @param context[in,out] The ECS context.
 * @param component[in,out] The TransformComponent to update.
 * @param z[in] The new Z-index.
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_TransformComponent_setZIndex(INOUT CE_ECS_Context* context, INOUT CE_TransformComponent* component, IN int16_t z);

/**
 * @brief Function-like macro: Check if the TransformComponent has the specified flags set.
 * 
 * @param component[in] Pointer to the TransformComponent to check.
 * @param flags[in] The flags to check for.
 * @return true if all the specified flags are set, false otherwise.
 */
#define CE_TransformComponent_checkFlags(component, flags) (((component)->m_flags & (flags)) == (flags))

/**
 * @brief Function-like macro: Set the specified flags on the TransformComponent.
 * 
 * @param component[in,out] Pointer to the TransformComponent to update.
 * @param flags[in] The flags to set.
 */
#define CE_TransformComponent_setFlags(component, flags) ((component)->m_flags |= (flags))

/**
 * @brief Function-like macro: Clear the specified flags on the TransformComponent.
 * 
 * @param component[in,out] Pointer to the TransformComponent to update.
 * @param flags[in] The flags to clear.
 */
#define CE_TransformComponent_clearFlags(component, flags) ((component)->m_flags &= ~(flags))

/**
 * @brief Function-like macro: Check if the TransformComponent is currently in the scene graph.
 * 
 * @param component[in] Pointer to the TransformComponent to check.
 * @return true if the component is in the scene graph, false otherwise.
 */
#define CE_TransformComponent_isInSceneGraph(component) (((component)->m_flags & CE_TransformComponent_Flags_InSceneGraph) != 0)

/**
 * @brief Function-like macro: Check if the TransformComponent is dirty and needs to be updated in the scene graph.
 * This is currently not used, but it can be used in the future to optimize scene graph updates by only updating components that have changed.
 * 
 * @param component[in] Pointer to the TransformComponent to check.
 * @return true if the component is dirty, false otherwise.
 */
#define CE_TransformComponent_isDirty(component) (((component)->m_flags & CE_TransformComponent_Flags_Dirty) != 0)

/**
 * @brief Function-like macro: Check if the TransformComponent has a fixed position.
 * 
 * @param component[in] Pointer to the TransformComponent to check.
 * @return true if the component has a fixed position, false otherwise.
 */
#define CE_TransformComponent_hasFixedPosition(component) (((component)->m_flags & CE_TransformComponent_Flags_FixedPosition) != 0)

#endif // CORGO_ENGINE_COMPONENTS_TRANSFORM_H
