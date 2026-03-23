//
//  ecs/core/component.h
//  Main include file for ECS core component definitions.
//  Not to be confused with components.h which is a helper with all component types.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_COMPONENT_H
#define CORGO_ECS_CORE_COMPONENT_H

#include "../types.h"

struct CE_ECS_ComponentStaticData {
    bool m_isValid;
    CE_TypeId m_type;
    uint16_t m_uid;
    uint16_t m_initialCapacity;
    size_t m_storageSizeOf;
    
    // Component methods
    CE_Result (*m_initFunction)(INOUT CE_ECS_Context* context, INOUT void* component);
    CE_Result (*m_cleanupFunction)(INOUT CE_ECS_Context* context, INOUT void* component);
};

// Declare a dummy component type for components that don't require storage, to avoid null pointers and simplify logic.
typedef struct CE_NoStorageComponent {
    uint8_t dummy; // Placeholder to ensure the struct is not empty, as C does not allow empty structs.
} CE_NoStorageComponent;

#define CE_NO_STORAGE_COMPONENT_STORAGE CE_NoStorageComponent

// Macro to help define a no storage component, adds some hidden data we'll use later
#define CE_NS_COMPONENT_DESC(name, uid) CE_COMPONENT_DESC(name, uid, CE_NO_STORAGE_COMPONENT_STORAGE, 0, CE_NO_STORAGE_COMPONENT_GEN(name))

//// Component macros

// Component method shortcuts
#define CE_DEFINE_COMPONENT_INIT(name) CE_Result name##_init(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component)
#define CE_DEFINE_COMPONENT_CLEANUP(name) CE_Result name##_cleanup(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component)

// Component data shortcuts
#define CE_COMPONENT_DATA(name) name##_StorageType
#define CE_COMPONENT_UID(name) name##_UID

// Main component declaration, declares all methods and storage for a component.
// UID is a unique int32 identifier for the component type, it must be unique across all components and hardcoded.
// Changing of uuid is not allowed once a component is in use, as it will break serialization.
// UIDs 0-10 are reserved for core components.
#define CE_DECLARE_COMPONENT(name, c_uid, storage, initial_capacity, ...) \
void name##_description(OUT CE_ECS_ComponentStaticData *data);\
typedef storage CE_COMPONENT_DATA(name);\
static const uint16_t CE_COMPONENT_UID(name) = c_uid;\
CE_DEFINE_COMPONENT_INIT(name); \
CE_DEFINE_COMPONENT_CLEANUP(name); \
CE_Result name##_init_wrapper(INOUT CE_ECS_Context* context, INOUT void* component);\
CE_Result name##_cleanup_wrapper(INOUT CE_ECS_Context* context, INOUT void* component);\
_Static_assert(initial_capacity <= CE_BITSET_MAX_BITS, #name ": Component initial capacity exceeds bitset max bits, increase CE_BITSET_MAX_BITS or reduce initial capacity.");



// Component method implementation generator
// Expanded from ecs/core/component_registry.c for all registered components
#define CE_GENERATE_COMPONENT_IMP(name, c_uid, storage, initial_capacity, ...) \
CE_Result name##_init_wrapper(INOUT CE_ECS_Context* context, INOUT void* component) \
{ \
    if (!component) return CE_ERROR; \
    return name##_init(context, (name##_StorageType*)component); \
} \
CE_Result name##_cleanup_wrapper(INOUT CE_ECS_Context* context, INOUT void* component) \
{ \
    if (!component) return CE_ERROR; \
    return name##_cleanup(context, (name##_StorageType*)component); \
} \
void name##_description(OUT CE_ECS_ComponentStaticData *data) \
{ \
    data->m_isValid = true; \
    data->m_type = name; \
    data->m_uid = c_uid; \
    data->m_storageSizeOf = sizeof(storage); \
    data->m_initialCapacity = initial_capacity; \
    data->m_initFunction = name##_init_wrapper; \
    data->m_cleanupFunction = name##_cleanup_wrapper; \
} \
__VA_ARGS__\

//// Global component macros
#define CE_GLOBAL_COMPONENT(name) CE_GLOBAL_##name
#define CE_GLOBAL_COMPONENT_DATA(name) CE_PASTE(CE_GLOBAL_COMPONENT(name), _StorageType)
#define CE_GLOBAL_COMPONENT_INIT_FUNCTION(name) CE_PASTE(CE_GLOBAL_COMPONENT(name), _init)
#define CE_GLOBAL_COMPONENT_CLEANUP_FUNCTION(name) CE_PASTE(CE_GLOBAL_COMPONENT(name), _cleanup)

// Component method shortcuts
#define CE_DEFINE_GLOBAL_COMPONENT_INIT(name) CE_Result CE_GLOBAL_COMPONENT_INIT_FUNCTION(name)(INOUT CE_ECS_Context* context, INOUT CE_GLOBAL_COMPONENT_DATA(name)* component)
#define CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(name) CE_Result CE_GLOBAL_COMPONENT_CLEANUP_FUNCTION(name)(INOUT CE_ECS_Context* context, INOUT CE_GLOBAL_COMPONENT_DATA(name)* component)

#define CE_DECLARE_GLOBAL_COMPONENT(name, storage) \
typedef storage CE_PASTE(CE_GLOBAL_COMPONENT(name), _StorageType);\
CE_DEFINE_GLOBAL_COMPONENT_INIT(name); \
CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(name);



#endif // CORGO_ECS_CORE_COMPONENT_H