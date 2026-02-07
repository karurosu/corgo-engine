//
//  ecs/core/error.h
//  Main include file for error handling macros and types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_ERROR_H
#define CORGO_ECS_CORE_ERROR_H

typedef int CE_Result;

//// Result codes
// Generic success code
#define CE_OK 0
// Generic error code
#define CE_ERROR -1


// X-macro pattern for error code definitions
// Format: X(EnumName, ErrorCode, ErrorMessage)
// Enum names must be in the form of SYSTEM_SUBSYSTEM_DESCRIPTION
// Enums 0-9: General errors
// Enums 10-19: Main Storage related errors
// Enums 20-29: Component Storage related errors
// Enums 30-39: Entity related errors
// Enums 40-49: Component related errors
// Enums 50-59: Relationship related errors
// Enums 60-69: bitset errors
// Enums 70-99: Engine specific errors
// Enums 100+: Game specific errors
#define CE_ERROR_CODE_LIST(X) \
    X(NONE, 0, "No error") \
    X(UNSUPPORTED_OPERATION, 1, "Unsupported operation") \
    X(OUT_OF_MEMORY, 2, "Out of memory") \
    X(INTERNAL_ERROR, 3, "Internal error") \
    \
    X(STORAGE_MAIN_NOT_INITIALIZED, 10, "Main Storage not initialized") \
    X(STORAGE_MAIN_ALREADY_INITIALIZED, 11, "Main Storage already initialized") \
    X(STORAGE_MAIN_ALLOCATION_FAILED, 12, "Main storage allocation failed") \
    \
    X(STORAGE_COMPONENT_NOT_INITIALIZED, 20, "Component storage not initialized") \
    X(STORAGE_COMPONENT_ALREADY_INITIALIZED, 21, "Component storage already initialized") \
    X(STORAGE_COMPONENT_ALLOCATION_FAILED, 22, "Component storage allocation failed") \
    X(STORAGE_COMPONENT_INIT_FAILED, 23, "Component storage initialization failed") \
    X(STORAGE_COMPONENT_CLEANUP_FAILED, 24, "Component storage cleanup failed") \
    X(STORAGE_COMPONENT_BITSET_TOO_SMALL, 25, "Component storage bitset too small") \
    X(STORAGE_COMPONENT_NOT_FOUND, 26, "Component not found in storage") \
    \
    X(MAX_ENTITIES_REACHED, 30, "Maximum number of entities reached") \
    X(INVALID_ENTITY_ID, 31, "Invalid entity ID") \
    X(ENTITY_NOT_FOUND, 32, "Entity not found") \
    X(STALE_ENTITY_ID, 33, "Entity ID references a deleted entity") \
    \
    X(INVALID_COMPONENT_TYPE, 40, "Invalid component type") \
    X(COMPONENT_NOT_FOUND_IN_ENTITY, 41, "Component not found in entity") \
    X(INVALID_COMPONENT_ID, 42, "Invalid component ID") \
    \
    X(INVALID_RELATIONSHIP_TYPE, 50, "Invalid relationship type") \
    X(ENTITY_DOES_NOT_HAVE_RELATIONSHIP, 51, "Entity does not have the specified relationship") \
    \
    X(BITSET_INDEX_OUT_OF_BOUNDS, 60, "Bitset index out of bounds") \
    \
    X(ENGINE_CANNOT_INITIALIZE_SCENE_GRAPH, 70, "Engine cannot initialize scene graph") \
    X(ENGINE_ALREADY_PARENTED, 71, "New child entity already has a parent") \
    X(ENGINE_ENTITY_DOES_NOT_HAVE_CHILD, 72, "Entity does not have the specified child") \
    /* Add new error codes here */

// Generate enum values
typedef enum CE_ERROR_CODE_E {
#define CE_ERROR_ENUM(name, code, msg) CE_ERROR_CODE_##name = code,
    CE_ERROR_CODE_LIST(CE_ERROR_ENUM)
#undef CE_ERROR_ENUM
    CE_ERROR_CODE_COUNT
} CE_ERROR_CODE;

// Get error message string for an error code
const char* CE_GetErrorMessage(CE_ERROR_CODE errorCode);

// Set the output error code if the pointer is not null
// Only used for core functions that have an OUT CE_ERROR_CODE* parameter
// User code should use the debug component
#define CE_SET_ERROR_CODE(outErrorCode, code) \
    do { \
        if (outErrorCode) { \
            *(outErrorCode) = (code); \
        } \
    } while(0)

#endif // CORGO_ECS_CORE_ERROR_H