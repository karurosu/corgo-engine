//
//  ecs/core/types.c
//  CE_Id utilities implementation
//

#include "id.h"

static inline uint32_t clamp6(uint32_t v) { return v & CE_ID_MASK_TYPE6; }
static inline uint32_t clamp16(uint32_t v) { return v & CE_ID_MASK_UNIQUE; }

CE_Result CE_Id_setKind(OUT CE_Id* id, CE_IdKind kind)
{
    if (!id) return CE_ERROR;
    CE_Id cur = *id;
    switch (kind) {
    case CE_ID_ENTITY_REFERENCE_KIND:
        // Clear component and set relationship off
        *id = (cur & ~(CE_ID_FLAG_COMPONENT | CE_ID_FLAG_RELATIONSHIP));
        return CE_OK;
    case CE_ID_COMPONENT_REFERENCE_KIND:
        // Set component, clear relationship
        *id = (cur | CE_ID_FLAG_COMPONENT) & ~CE_ID_FLAG_RELATIONSHIP;
        return CE_OK;
    case CE_ID_ENTITY_RELATIONSHIP_KIND:
        // Ensure entity (no component) and set relationship
        *id = (cur & ~CE_ID_FLAG_COMPONENT) | CE_ID_FLAG_RELATIONSHIP;
        return CE_OK;
    default:
        return CE_ERROR;
    }
}

CE_Result CE_Id_setUniqueId(OUT CE_Id* id, uint32_t uniqueId)
{
    if (!id) return CE_ERROR;
    if (uniqueId > CE_ID_MASK_UNIQUE) return CE_ERROR;

    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_UNIQUE << CE_ID_SHIFT_UNIQUE)) | ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);
    return CE_OK;
}

CE_Result CE_Id_setGeneration(OUT CE_Id* id, uint32_t generation)
{
    if (!id) return CE_ERROR;
    if (generation > CE_ID_MASK_GENERATION) return CE_ERROR;

    if (!CE_Id_isEntity(*id)) return CE_ERROR;
    
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_GENERATION << CE_ID_SHIFT_GENERATION)) | ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
    return CE_OK;
}

CE_Result CE_Id_setComponentTypeId(OUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return CE_ERROR;
    if (!CE_Id_isComponent(*id)) return CE_ERROR;

    if ((uint32_t)typeId > CE_ID_MASK_TYPE6) return CE_ERROR;
    
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE6 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
    return CE_OK;
}

CE_Result CE_Id_setRelationshipTypeId(OUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return CE_ERROR;
    if (!CE_Id_isRelationship(*id)) return CE_ERROR;

    if ((uint32_t)typeId > CE_ID_MASK_TYPE6) return CE_ERROR;

    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE6 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
    return CE_OK;
}

CE_Result CE_Id_make(CE_IdKind kind, CE_TypeId typeId, uint32_t generation, uint32_t uniqueId, OUT CE_Id* out)
{
    if (!out) return CE_ERROR;
    
    // Validate widths
    if (uniqueId > CE_ID_MASK_UNIQUE) 
    { 
        *out = CE_INVALID_ID; 
        return CE_ERROR; 
    }

    // Check generation for entity kinds only
    if ((kind == CE_ID_ENTITY_REFERENCE_KIND || kind == CE_ID_ENTITY_RELATIONSHIP_KIND) && generation > CE_ID_MASK_GENERATION) 
    { 
        *out = CE_INVALID_ID; 
        return CE_ERROR; 
    }

    if (kind == CE_ID_COMPONENT_REFERENCE_KIND || kind == CE_ID_ENTITY_RELATIONSHIP_KIND) {
        if ((uint32_t)typeId > CE_ID_MASK_TYPE6) { *out = CE_INVALID_ID; return CE_ERROR; }
    }

    CE_Id id = 0;
    // Set unique id
    id |= ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);

    // Set flags and fields based on kind
    switch (kind) {
    case CE_ID_ENTITY_REFERENCE_KIND:
        // entity direct
        // ensure flags indicate entity direct
        // generation applies
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
        break;
    case CE_ID_COMPONENT_REFERENCE_KIND:
        // component
        id |= CE_ID_FLAG_COMPONENT;
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
        break;
    case CE_ID_ENTITY_RELATIONSHIP_KIND:
        // entity relationship
        id |= CE_ID_FLAG_RELATIONSHIP;
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
        break;
    default:
        *out = CE_INVALID_ID;
        return CE_ERROR;
    }

    *out = id;
    return CE_OK;
}

bool CE_Id_compare(IN CE_Id a, IN CE_Id b)
{
    // different kinds cannot be equal
    if (CE_Id_getKind(a) != CE_Id_getKind(b)) return false;

    // For entities compare generations first
    if (CE_Id_isEntity(a) && CE_Id_getGeneration(a) != CE_Id_getGeneration(b)) {
        return false;
    }

    // Otherwise, compare components directly
    return CE_Id_getUniqueId(a) == CE_Id_getUniqueId(b);
}
