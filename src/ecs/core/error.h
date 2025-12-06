//
//  ecs/core/error.h
//  Main include file for error handling macros and types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_ERROR_H
#define CORGO_ECS_CORE_ERROR_H

//// Result codes
// Generic success code
#define CE_OK 0
// Generic error code
#define CE_ERROR -1
typedef int CE_Result;

// X-macro pattern for error code definitions
// Format: X(EnumName, ErrorCode, ErrorMessage)
#define CE_ERROR_CODE_LIST(X) \
    X(NONE, 0, "No error") \
    X(STORAGE_ALREADY_INITIALIZED, 1, "Storage already initialized") \
    X(STORAGE_MAIN_ALLOCATION_FAILED, 2, "Storage main allocation failed") \
    X(STORAGE_COMPONENT_DATA_POOL_ALLOCATION_FAILED, 3, "Storage component data pool allocation failed") \
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