//
//  ecs/core/ecs_component.h
//  Live Component manipulation functions.
//  Aggregates core ECS headers for easy inclusion.
//  Copyright (c) 2026 Carlos Camacho.
//

#ifndef CORGO_ECS_CORE_ECS_COMPONENT_H
#define CORGO_ECS_CORE_ECS_COMPONENT_H

#include "ecs/types.h"

////////////////////////////////////
// Component Manipulation
////////////////////////////////////

/**
 * @brief Add a component to an entity.
 * 
 * Creates a new component instance of the specified type and attaches it
 * to the given entity. The component is initialized with default values.
 * The returned component ID is valid only for the lifetime of the entity and should not be shared with other entities.
 * The component data pointer, if provided, will point to the newly created component's data and can be used to initialize or modify the component.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to add the component to.
 * @param[in] componentType The type ID of the component to add.
 * @param[out] componentId Pointer to receive the ID of the new component instance.
 * @param[out] componentData Optional pointer to receive the address of the component data.
 * @param[out] errorCode Optional error code if addition fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., entity already has this component type).
 */
CE_Result CE_Entity_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id* componentId, OUT_OPT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Remove a component from an entity.
 * 
 * Detaches the specified component from an entity and releases its data.
 * Any pointers to the component data become invalid after this call.
 * The component ID becomes invalid after this call but may be reused for future components so it should not be cached. 
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to remove the component from.
 * @param[in] componentId The ID of the component to remove.
 * @param[out] errorCode Optional error code if removal fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component not found).
 */
CE_Result CE_Entity_RemoveComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT_OPT CE_ERROR_CODE* errorCode);

////////////////////////////////////
/// Component data access functions
////////////////////////////////////

/**
 * @brief Retrieve a component instance from an entity.
 * 
 * Looks up the component for the specified component ID on the given entity.
 * This access is very fast as it only involves a direct lookup and does not require iteration over all components.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity containing the component.
 * @param[in] componentId The ID of the component to retrieve.
 * @param[out] componentData Pointer to receive the address of the component data. Do not save or cache this pointer.
 * @param[out] errorCode Optional error code if retrieval fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component not found).
 */
CE_Result CE_Entity_GetComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_Id componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Find the first component of a specific type on an entity.
 * 
 * Searches for the first component matching the given type ID on the entity.
 * If the entity has multiple components of the same type, only the first is returned.
 * The order of the components is not guaranteed, use this if you are sure there is only one component of this type on the entity.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the component to find.
 * @param[out] componentId Pointer to receive the ID of the found component.
 * @param[out] componentData Pointer to receive the address of the component data. Do not save or cache this pointer.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure (e.g., component type not found).
 */
CE_Result CE_Entity_FindFirstComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id* componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);
    
/**
 * @brief Find all components of a specific type on an entity.
 * 
 * Searches for all components matching the given type ID on the entity
 * and returns their IDs in the provided buffer.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the components to find.
 * @param[out] results Buffer to receive an array of component IDs.
 * @param[in] bufsize Maximum number of component IDs to return.
 * @param[out] resultCount The actual number of components found.
 * @param[out] errorCode Optional error code if search fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 * 
 * @note If the number of components exceeds bufsize, only bufsize results are returned
 *       and resultCount will contain the actual count found.
 */
CE_Result CE_Entity_FindAllComponents(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id results[], IN size_t bufsize, OUT size_t *resultCount, OUT_OPT CE_ERROR_CODE* errorCode);

/**
 * @brief Retrieve a cc_set reference to all components of an entity.
 * 
 * The returned container is the raw data, so it should not be altered unless you know what you are doing.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] entity The ID of the entity to search.
 * @param[in] componentType The type ID of the components to find.
 * @param[out] components Pointer to receive the set of component IDs. Do not modify the returned set.
 * @param[out] errorCode Optional error code if retrieval fails.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Entity_GetAllComponentsIter(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, OUT CE_Id_Set **components, OUT_OPT CE_ERROR_CODE* errorCode);

////////////////////////////////////
/// Global component access functions
////////////////////////////////////

/**
 * @brief Access a global component by name.
 * Function-like macro, returns the name of the global component data. Must be cast to a pointer of the appropriate type.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] name The name of the global component to access.
 * @return Pointer to the global component data.
 */
#define CE_ECS_AccessGlobalComponent(context, name) &((context)->m_storage.m_globalComponents.CE_PASTE(m_, CE_GLOBAL_COMPONENT(name)))

/**
 * @brief Access a global component by name and create a pointer of the appropriate type with the specified name
 * Function-like macro, must be called on its own line as it defines a new pointer variable.
 * 
 * @param[in,out] context The ECS context.
 * @param[in] name The name of the global component to access.
 * @param[in] varname to store the typed pointer to the global component data.
 */
#define CE_ECS_AccessGlobalComponentToVariable(context, name, varname) CE_GLOBAL_COMPONENT_DATA(name)* varname = CE_ECS_AccessGlobalComponent(context, name)

////////////////////////////////////
/// Internal ECS functions, exposed here because user code may need to call them
/// But regular should not use it directly
////////////////////////////////////

CE_Result CE_ECS_GetComponentForSystem(INOUT CE_ECS_Context* context, IN CE_Id entity, IN CE_TypeId componentType, const IN CE_ECS_SystemStaticData *system, OUT CE_Id* componentId, OUT void **componentData, OUT_OPT CE_ERROR_CODE* errorCode);


#endif // CORGO_ECS_CORE_ECS_COMPONENT_H