//
//  ecs/core/types.c
//  CE_Id utilities implementation
//

#include "id.h"

static inline uint32_t clamp8(uint32_t v) { return v & CE_ID_MASK_TYPE8; }
static inline uint32_t clamp16(uint32_t v) { return v & CE_ID_MASK_UNIQUE; }

CE_Result CE_Id_setKind(INOUT CE_Id* id, CE_IdKind kind)
{
    if (!id) return CE_ERROR;
    if (kind == CE_ID_INVALID_KIND) return CE_ERROR;
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_KIND << CE_ID_SHIFT_KIND)) | (((uint32_t)kind & CE_ID_MASK_KIND) << CE_ID_SHIFT_KIND);
    return CE_OK;
}

CE_Result CE_Id_setUniqueId(INOUT CE_Id* id, uint32_t uniqueId)
{
    if (!id) return CE_ERROR;
    if (uniqueId > CE_ID_MASK_UNIQUE) return CE_ERROR;

    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_UNIQUE << CE_ID_SHIFT_UNIQUE)) | ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);
    return CE_OK;
}

CE_Result CE_Id_setGeneration(INOUT CE_Id* id, uint32_t generation)
{
    if (!id) return CE_ERROR;
    if (generation > CE_ID_MASK_GENERATION) return CE_ERROR;

    if (CE_Id_isComponent(*id)) return CE_ERROR;
    
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_GENERATION << CE_ID_SHIFT_GENERATION)) | ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
    return CE_OK;
}

CE_Result CE_Id_setComponentTypeId(INOUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return CE_ERROR;
    if (!CE_Id_isComponent(*id)) return CE_ERROR;

    if ((uint32_t)typeId > CE_ID_MASK_TYPE8) return CE_ERROR;
    
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE8 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE8) << CE_ID_SHIFT_TYPE);
    return CE_OK;
}

CE_Result CE_Id_setRelationshipTypeId(INOUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return CE_ERROR;
    if (!CE_Id_isRelationship(*id)) return CE_ERROR;

    if ((uint32_t)typeId > CE_ID_MASK_TYPE8) return CE_ERROR;

    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE8 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE8) << CE_ID_SHIFT_TYPE);
    return CE_OK;
}

CE_Result CE_Id_make(IN CE_IdKind kind, IN CE_TypeId typeId, IN uint32_t generation, IN uint32_t uniqueId, OUT CE_Id* out)
{
    if (!out) return CE_ERROR;
    
    // Validate widths
    if (uniqueId > CE_ID_MASK_UNIQUE) 
    { 
        *out = CE_INVALID_ID; 
        return CE_ERROR; 
    }

    if (kind == CE_ID_INVALID_KIND || kind >= CE_ID_KIND_COUNT) { *out = CE_INVALID_ID; return CE_ERROR; }

    // Validate generation for entity kind only
    if ((kind == CE_ID_ENTITY_REFERENCE_KIND || kind == CE_ID_ENTITY_RELATIONSHIP_KIND) && generation > CE_MAX_GENERATION_COUNT) { *out = CE_INVALID_ID; return CE_ERROR; }

    // Validate 8-bit type for component/relationship kinds
    if ((kind == CE_ID_COMPONENT_REFERENCE_KIND || kind == CE_ID_ENTITY_RELATIONSHIP_KIND) && (uint32_t)typeId >= CE_INVALID_TYPE_ID) {
        *out = CE_INVALID_ID; return CE_ERROR;
    }

    CE_Id id = 0;
    // Set unique id
    id |= ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);

    // Set kind field
    id |= (((uint32_t)kind & CE_ID_MASK_KIND) << CE_ID_SHIFT_KIND);

    // Set fields based on kind
    switch (kind) {
    case CE_ID_ENTITY_REFERENCE_KIND:
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
        break;
    case CE_ID_COMPONENT_REFERENCE_KIND:
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE8) << CE_ID_SHIFT_TYPE);
        break;
    case CE_ID_ENTITY_RELATIONSHIP_KIND:
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE8) << CE_ID_SHIFT_TYPE);
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
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
