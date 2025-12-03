//
//  ecs/core/types.c
//  CE_Id utilities implementation
//

#include "../types.h"

static inline uint32_t clamp6(uint32_t v) { return v & CE_ID_MASK_TYPE6; }
static inline uint32_t clamp16(uint32_t v) { return v & CE_ID_MASK_UNIQUE; }

int CE_Id_setFlags(OUT CE_Id* id, CE_IdKind kind)
{
    if (!id) return -1;
    CE_Id cur = *id;
    switch (kind) {
    case CE_ID_ENTITY_REFERENCE:
        // Clear component and set relationship off
        *id = (cur & ~(CE_ID_FLAG_COMPONENT | CE_ID_FLAG_RELATIONSHIP));
        return 0;
    case CE_ID_COMPONENT_REFERENCE:
        // Set component, clear relationship
        *id = (cur | CE_ID_FLAG_COMPONENT) & ~CE_ID_FLAG_RELATIONSHIP;
        return 0;
    case CE_ID_ENTITY_RELATIONSHIP:
        // Ensure entity (no component) and set relationship
        *id = (cur & ~CE_ID_FLAG_COMPONENT) | CE_ID_FLAG_RELATIONSHIP;
        return 0;
    default:
        return -1;
    }
}

int CE_Id_setUniqueId(OUT CE_Id* id, uint32_t uniqueId)
{
    if (!id) return -1;
    if (uniqueId > CE_ID_MASK_UNIQUE) return -1;
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_UNIQUE << CE_ID_SHIFT_UNIQUE)) | ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);
    return 0;
}

int CE_Id_setGeneration(OUT CE_Id* id, uint32_t generation)
{
    if (!id) return -1;
    if (generation > CE_ID_MASK_GENERATION) return -1;
    if (!CE_Id_isEntity(*id)) return -1;
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_GENERATION << CE_ID_SHIFT_GENERATION)) | ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
    return 0;
}

int CE_Id_setComponentTypeId(OUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return -1;
    if (!CE_Id_isComponent(*id)) return -1;
    if ((uint32_t)typeId > CE_ID_MASK_TYPE6) return -1;
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE6 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
    return 0;
}

int CE_Id_setRelationshipTypeId(OUT CE_Id* id, CE_TypeId typeId)
{
    if (!id) return -1;
    if (!CE_Id_isRelationship(*id)) return -1;
    if ((uint32_t)typeId > CE_ID_MASK_TYPE6) return -1;
    CE_Id cur = *id;
    *id = (cur & ~(CE_ID_MASK_TYPE6 << CE_ID_SHIFT_TYPE)) | (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
    return 0;
}

int CE_Id_make(CE_IdKind kind, CE_TypeId typeId, uint32_t generation, uint32_t uniqueId, OUT CE_Id* out)
{
    if (!out) return -1;
    // Validate widths
    if (uniqueId > CE_ID_MASK_UNIQUE) { *out = CE_INVALID_ID; return -1; }
    // For entity kinds, generation must be within 6 bits; for component kinds, generation ignored but validate input for consistency
    if (generation > CE_ID_MASK_GENERATION) { *out = CE_INVALID_ID; return -1; }
    if (kind == CE_ID_COMPONENT_REFERENCE || kind == CE_ID_ENTITY_RELATIONSHIP) {
        if ((uint32_t)typeId > CE_ID_MASK_TYPE6) { *out = CE_INVALID_ID; return -1; }
    }

    CE_Id id = 0;
    // Set unique id
    id |= ((uniqueId & CE_ID_MASK_UNIQUE) << CE_ID_SHIFT_UNIQUE);

    // Set flags and fields based on kind
    switch (kind) {
    case CE_ID_ENTITY_REFERENCE:
        // entity direct
        // ensure flags indicate entity direct
        // generation applies
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
        break;
    case CE_ID_COMPONENT_REFERENCE:
        // component
        id |= CE_ID_FLAG_COMPONENT;
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
        break;
    case CE_ID_ENTITY_RELATIONSHIP:
        // entity relationship
        id |= CE_ID_FLAG_RELATIONSHIP;
        id |= ((generation & CE_ID_MASK_GENERATION) << CE_ID_SHIFT_GENERATION);
        id |= (((uint32_t)typeId & CE_ID_MASK_TYPE6) << CE_ID_SHIFT_TYPE);
        break;
    default:
        *out = CE_INVALID_ID;
        return -1;
    }

    *out = id;
    return 0;
}

bool CE_Id_compare(IN CE_Id a, IN CE_Id b)
{
    // Components: compare unique id only
    if (CE_Id_isComponent(a) || CE_Id_isComponent(b)) {
        // If either is component, both must be component to be comparable by our rule
        if (!(CE_Id_isComponent(a) && CE_Id_isComponent(b))) return false;
        return CE_Id_getUniqueId(a) == CE_Id_getUniqueId(b);
    }

    // Entities (direct or relationship): compare unique id and generation
    return (CE_Id_getUniqueId(a) == CE_Id_getUniqueId(b)) && (CE_Id_getGeneration(a) == CE_Id_getGeneration(b));
}
