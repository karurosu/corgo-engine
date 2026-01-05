//
//  ecs/core/error.c
//  Main include file for error handling macros and types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#include "error.h"

const char* CE_GetErrorMessage(CE_ERROR_CODE errorCode) {
    switch (errorCode) {
#define CE_ERROR_MESSAGE_CASE(name, code, msg) case CE_ERROR_CODE_##name: return msg;
        CE_ERROR_CODE_LIST(CE_ERROR_MESSAGE_CASE)
#undef CE_ERROR_MESSAGE_CASE
        default: return "Unknown error";
    }
}