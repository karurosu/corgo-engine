//
//  engine/core/shortcuts.h
//  These are macro shortcuts to make it easier to write scene scripts, they are optional to use but can make code more legible.
//  These may not work outside of scene scripts.
//  Shortcuts are not included by default.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ENGINE_CORE_SHORTCUTS_H
#define CORGO_ENGINE_CORE_SHORTCUTS_H

// Entity creation shortcut
#define CES_CREATE_ENTITY(name)\
    CE_Id name = CE_INVALID_ID;\
    if (CE_ECS_CreateEntity(context, &name, errorCode) != CE_OK) {\
        CE_Error("Failed to create entity: " #name " Error: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Component shortcuts
#define CES_ADD_COMPONENT(entity, component)\
    if (CE_Entity_AddComponent(context, entity, component, NULL, NULL, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

#define CES_ADD_COMPONENT_PTR(entity, component, ptr)\
    component##_StorageType *ptr = NULL;\
    if (CE_Entity_AddComponent(context, entity, component, NULL, (void**)&ptr, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

#define CES_ADD_COMPONENT_EPTR(entity, component, ptr)\
    if (CE_Entity_AddComponent(context, entity, component, NULL, (void**)&ptr, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #component " to entity: %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Generic error handling
#define CES_CHECK_RESULT(result, msg) \
    if ((result) != CE_OK) {\
        CE_Error(msg ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

// Scene tree handling
#define CES_ROOT_ENTITY CE_Scene_GetRootId(context)

#define CES_ADD_CHILD(parent, child) \
    if (CE_Scene_AddChild(context, parent, child, false, errorCode) != CE_OK) {\
        CE_Error("Failed to add " #child " entity to " #parent ": %s", CE_GetErrorMessage(*errorCode));\
        return CE_ERROR;\
    }

#endif // CORGO_ENGINE_CORE_SHORTCUTS_H