//
//  engine/shortcuts/scene.h
//  These are macro shortcuts to make it easier to write scene scripts, they are optional to use but can make code more legible.
//  These may not work outside of scene scripts.
//  Shortcuts are not included by default.
//  Using CES_ prefix to separate them from engine API, CES = Corgo Engine Shortcuts.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_SHORTCUTS_SCENE_H
#define CORGO_ENGINE_SHORTCUTS_SCENE_H

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

/**
 * @brief Macro to add an entity to the root of the scene graph.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param child[in] The child entity ID.
 * @return void
 */
#define CES_ADD_TO_ROOT(child) CES_ADD_CHILD(CES_ROOT_ENTITY, child)

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


// Input shortcuts

/**
 * @brief Macro to access the input component in a scene script, must be called before reading input states.
 * 
 * @return void
 */
#define CES_INITIALIZE_INPUT() CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_COMPONENT, inputComponent);

/**
 * @brief Helper macro to check if an input action is active.
 * 
 * @param action[in] The input action to check.
 * @return true if the action is active, false otherwise.
 */
#define CES_IA_ISACTIVE(action) (inputComponent->m_actionStates & CE_IA_MASK(action))

/**
 * @brief Helper macro to check if an input action is not active.
 * 
 * @param action[in] The input action to check.
 * @return true if the action is not active, false otherwise.
 */
#define CES_IA_ISNOTACTIVE(action) ((inputComponent->m_actionStates & CE_IA_MASK(action)) == 0)

/**
 * @brief Helper macro to check if an input button is currently pressed in the raw input state.
 * Input Actions are preferred, but this is exposed for simplicity
 * 
 * @param button[in] The input button bitmask to check, must be a valid CE_Input_FeatureMask value.
 * @return true if the button is currently pressed, false otherwise.
 */
#define CES_INPUT_IS_PRESSED(button) (inputComponent->m_rawInputsCurrent & button)

/**
 * @brief Helper macro to check if an input button was just pressed in the raw input state.
 * Input Actions are preferred, but this is exposed for simplicity
 * 
 * @param button[in] The input button bitmask to check, must be a valid CE_Input_FeatureMask value.
 * @return true if the button was just pressed, false otherwise.
 */
#define CES_INPUT_WAS_JUST_PRESSED(button) (inputComponent->m_rawInputsPressed & button)

/**
 * @brief Helper macro to check if an input button was just released in the raw input state.
 * Input Actions are preferred, but this is exposed for simplicity
 * 
 * @param button[in] The input button bitmask to check, must be a valid CE_Input_FeatureMask value.
 * @return true if the button was just released, false otherwise.
 */
#define CES_INPUT_WAS_JUST_RELEASED(button) (inputComponent->m_rawInputsReleased & button)

/**
 * @brief Macro to enable a specific input feature.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param feature[in] The input feature to enable, must be a valid CE_Input_FeatureMask value.
 * @return void
 */
#define CES_ENABLE_INPUT_FEATURE(feature) \
    if (CE_Input_SetFeature(context, feature, true, errorCode) != CE_OK) {\
        CE_Error("Failed to enable input feature " #feature ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

/**
 * @brief Macro to disable a specific input feature.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param feature[in] The input feature to disable, must be a valid CE_Input_FeatureMask value.
 * @return void
 */
#define CES_DISABLE_INPUT_FEATURE(feature) \
    if (CE_Input_SetFeature(context, feature, false, errorCode) != CE_OK) {\
        CE_Error("Failed to disable input feature " #feature ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

/**
 * @brief Macro to check if the crank is currently docked, only valid if the crank input feature is enabled.
 * Convenience macro to avoid having a separate IA since it has a very specific meaning
 * 
 * @return true if the crank is docked or feature is not enabled (default is docked)
 */
#define CES_IS_CRANK_DOCKED() ((inputComponent->m_rawInputsCurrent & CE_Input_Crank) == 0)

/**
 * @brief Macro to get the current angle of the crank, only valid if the crank input feature is enabled.
 * 
 * @return The current angle of the crank in degrees, or 0 if the feature is not enabled.
 */
#define CE_GET_CRANK_ANGLE() (inputComponent->m_crankAngle)

/**
 * @brief Macro to get the current angle of the crank rounded to the nearest integer, only valid if the crank input feature is enabled.
 * 
 * @return The current angle of the crank in degrees rounded to the nearest integer, or 0 if the feature is not enabled.
 */
#define CE_GET_CRANK_ANGLE_ROUNDED() ((int)(inputComponent->m_crankAngle + 0.5f))

/**
 * @brief Macro to push a new input action map function onto the stack and make it the current action map.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @param map[in] The input action map function to push, must be of type CE_Input_ActionMapFunction and declared with CE_DECLARE_ACTION_MAP_FUNCTION.
 * @return void
 */
#define CES_PUSH_ACTION_MAP(map) \
    if (CE_Input_PushActionMap(context, map, errorCode) != CE_OK) {\
        CE_Error("Failed to push action map " #map ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

/**
 * @brief Macro to pop the current input action map function from the stack.
 * Prints error and returns CE_ERROR if the operation fails.
 * 
 * @return void
 */
#define CES_POP_ACTION_MAP() \
    if (CE_Input_PopActionMap(context, errorCode) != CE_OK) {\
        CE_Error("Failed to pop action map: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Scripting helpers
/**
 * @brief Macro to build a formatted string into a cc_string. Clears the string before formatting. If formatting fails, prints an error and returns CE_ERROR.
 * 
 * @param str[in,out] The cc_string to build the formatted string into. Must be a valid cc_string variable.
 * @param [var] List of strings or variables to append to the string
 */
#define CES_BUILD_STRING(str, ...)\
    cc_clear(&str);\
    if (!cc_push_fmt(&str, __VA_ARGS__)) {\
        CE_Error("Failed to format string: " #str);\
        return CE_ERROR;\
    }

#endif // CORGO_ENGINE_SHORTCUTS_SCENE_H
