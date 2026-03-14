//
//  engine/core/shortcuts.h
//  These are macro shortcuts to make it easier to write scene scripts, they are optional to use but can make code more legible.
//  These may not work outside of scene scripts.
//  Shortcuts are not included by default.
//  Using CES_ prefix to separate them from engine API, CES = Corgo Engine Shortcuts.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_SHORTCUTS_H
#define CORGO_ENGINE_CORE_SHORTCUTS_H

// Entity creation shortcut

/**
 * @brief Macro to create a new entity and check for errors.
 * Prints error and returns CE_ERROR if entity creation fails. Otherwise, the new entity ID is stored in the provided variable.
 * 
 * @param name[in] The name of the variable to hold the new entity ID. Must be a valid C identifier.
 * @return void
 */
#define CES_CREATE_ENTITY(name)\
    CE_Id name = CE_INVALID_ID;\
    if (CE_ECS_CreateEntity(context, &name, errorCode) != CE_OK) {\
        CE_Error("Failed to create entity: " #name " Error: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Component shortcuts

/**
 * @brief Macro to add a component to an entity without storing pointer or id.
 * Prints error and returns CE_ERROR if adding the component fails. Otherwise, the component is added to the entity.
 * 
 * @param entity[in] The entity ID to add the component to.
 * @param component[in] The component type to add.
 * @return void
 */
#define CES_ADD_COMPONENT(entity, component)\
    if (CE_Entity_AddComponent(context, entity, component, NULL, NULL, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

/**
 * @brief Macro to add a component to an entity and declare a new pointer of name 'ptr'.
 * Prints error and returns CE_ERROR if adding the component fails.
 * 
 * @param entity[in] The entity ID to add the component to.
 * @param component[in] The component type to add.
 * @param ptr[out] Name of the new pointer that will hold the component's data. Must be a valid C identifier.
 * @return void
 */
#define CES_ADD_COMPONENT_PTR(entity, component, ptr)\
    component##_StorageType *ptr = NULL;\
    if (CE_Entity_AddComponent(context, entity, component, NULL, (void**)&ptr, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

/**
 * @brief Macro to add a component to an entity and store the data to an existing pointer.
 * Prints error and returns CE_ERROR if adding the component fails.
 * The provided pointer must have been already declared.
 * 
 * @param entity[in] The entity ID to add the component to.
 * @param component[in] The component type to add.
 * @param ptr[out] A pointer that will now point to the new component's data. Must be a valid C identifier.
 * @return void
 */
#define CES_ADD_COMPONENT_EPTR(entity, component, ptr)\
    if (CE_Entity_AddComponent(context, entity, component, NULL, (void**)&ptr, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Generic error handling
/**
 * @brief Macro to check the result of an operation and print an error message if it failed.
 * 
 * @param result[in] Expression to check, must return a CE_Result.
 * @param msg[in] The error message to print if the operation failed, must be a string literal.
 * @return void
 */
#define CES_CHECK_RESULT(result, msg) \
    if ((result) != CE_OK) {\
        CE_Error(msg ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Scene tree handling

/**
 * @brief Macro to get the root entity ID of the scene graph.
 */
#define CES_ROOT_ENTITY CE_Scene_GetRootId(context)

/**
 * @brief Macro to add a child entity to a parent entity in the scene graph.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param parent[in] The parent entity ID.
 * @param child[in] The child entity ID.
 * @return void
 */
#define CES_ADD_CHILD(parent, child) \
    if (CE_Scene_AddChild(context, parent, child, false, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #child " entity to " #parent ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Scene script shortcuts

/**
 * @brief Macro to request loading a scene with the given name.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param name[in] The name of the scene to load, must be the same as the one used in CE_DECLARE_SCENE.
 * @return void
 */
#define CES_LOAD_SCENE(name) \
    if (CE_Scene_RequestLoad(context, CE_SCENE_LOAD_FUNCTION(name), errorCode) != CE_OK) {\
        CE_Error("Failed to load scene " #name ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

#endif // CORGO_ENGINE_CORE_SHORTCUTS_H