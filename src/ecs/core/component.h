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
    uint32_t m_uid;
    size_t m_storageSizeOf;
    uint32_t m_initialCapacity;

    // Component methods
    CE_Result (*m_initFunction)(INOUT CE_ECS_Context* context, INOUT void* component);
    CE_Result (*m_cleanupFunction)(INOUT CE_ECS_Context* context, INOUT void* component);
};

//// Component macros

// Component method shortcuts
#define CE_DEFINE_COMPONENT_INIT(name) CE_Result name##_init(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component)
#define CE_DEFINE_COMPONENT_CLEANUP(name) CE_Result name##_cleanup(INOUT CE_ECS_Context* context, INOUT name##_StorageType* component)

// Main component declaration, declares all methods and storage for a component.
// UID is a unique int32 identifier for the component type, it must be unique across all components and hardcoded.
// Changing of uuid is not allowed once a component is in use, as it will break serialization.
// UIDs 0-10 are reserved for core components.
#define CE_DECLARE_COMPONENT(name, c_uid, storage, initial_capacity) \
void name##_description(OUT CE_ECS_ComponentStaticData *data);\
typedef storage name##_StorageType;\
static const uint32_t name##_UID = c_uid;\
static const size_t name##_StorageSize = sizeof(storage);\
static const uint32_t name##_InitialCapacity = initial_capacity;\
CE_DEFINE_COMPONENT_INIT(name); \
CE_DEFINE_COMPONENT_CLEANUP(name); \
CE_Result name##_init_wrapper(INOUT CE_ECS_Context* context, INOUT void* component);\
CE_Result name##_cleanup_wrapper(INOUT CE_ECS_Context* context, INOUT void* component);\
_Static_assert(initial_capacity <= CE_BITSET_MAX_BITS, #name ": Component initial capacity exceeds bitset max bits, increase CE_BITSET_MAX_BITS or reduce initial capacity.");

// Component method implementation generator
// Must be called in the component's .c file
#define CE_GENERATE_COMPONENT_IMP(name) \
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
    data->m_uid = name##_UID; \
    data->m_storageSizeOf = name##_StorageSize; \
    data->m_initialCapacity = name##_InitialCapacity; \
    data->m_initFunction = name##_init_wrapper; \
    data->m_cleanupFunction = name##_cleanup_wrapper; \
}

// Component data shortcuts
#define CE_COMPONENT_DATA(name) name##_StorageType
#define CE_COMPONENT_UID(name) name##_UID

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