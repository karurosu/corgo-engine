//
//  ecs/core/component.h
//  Main include file for ECS core component definitions.
//  Not to be confused with components.h which is a helper with all component types.
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_ECS_CORE_COMPONENT_H
#define CORGO_ECS_CORE_COMPONENT_H

#include "../types.h"

struct CE_ECS_ComponentStaticData {
    bool m_isValid;
    CE_TypeId m_type;
    uint32_t m_uid;
    uint64_t m_bitmask;
    size_t m_storageSizeOf;
};

//// Component macros

// Main component declaration, declares all methods and storage for a component.
// UID is a unique int32 identifier for the component type, it must be unique across all components and hardcoded.
// Changing of uuid is not allowed once a component is in use, as it will break serialization.
// UIDs 0-10 are reserved for core components.
#define CE_DECLARE_COMPONENT(name, c_uid, storage) \
void name##_init(OUT storage* component); \
void name##_cleanup(OUT storage* component);\
void name##_description(OUT CE_ECS_ComponentStaticData *data);\
typedef storage name##_StorageType;

// Component method implementation generator
// Must be called in the component's .c file
#define CE_GENERATE_COMPONENT_IMP(name, c_uid, storage) \
void name##_description(OUT CE_ECS_ComponentStaticData *data) \
{ \
    data->m_isValid = true; \
    data->m_type = name; \
    data->m_uid = c_uid; \
    data->m_bitmask = ((uint64_t)1 << name); \
    data->m_storageSizeOf = sizeof(storage); \
}

// Component method declaration
#define CE_DEFINE_COMPONENT_INIT(name, storage) void name##_init(OUT storage* component);
#define CE_DEFINE_COMPONENT_CLEANUP(name, storage) void name##_cleanup(OUT storage* component);


#endif // CORGO_ECS_CORE_COMPONENT_H