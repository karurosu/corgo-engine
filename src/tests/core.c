#include "unity.h"

#include "ecs/types.h"
#include "ecs/ecs.h"
#include "utils/bitset.h"

static CE_ECS_Context context;

void setUp(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ECS_Init(&context, &errorCode);

    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
}

void tearDown(void) {
    // Clean up test fixtures
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ECS_Cleanup(&context, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
}


void test_ECSContextSetupTest(void) {
    CE_ERROR_CODE errorCode;
    
    // Verify we have at least 1 component type (CE_CORE_DEBUG_COMPONENT)
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(1, CE_COMPONENT_TYPES_COUNT);
    
    // Verify the debug component descriptor is valid
    CE_ECS_ComponentStaticData* debugDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    TEST_ASSERT_TRUE(debugDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_DEBUG_COMPONENT, debugDesc->m_type);
    TEST_ASSERT_EQUAL_UINT32(0, debugDesc->m_uid);
    TEST_ASSERT_EQUAL_UINT64((uint64_t)1 << CE_CORE_DEBUG_COMPONENT, debugDesc->m_bitmask);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), debugDesc->m_storageSizeOf);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), sizeof(CE_CORE_DEBUG_COMPONENT_StorageType));
    TEST_ASSERT_EQUAL_size_t(CE_DEFAULT_COMPONENT_CAPACITY, debugDesc->m_initialCapacity);
}

static void test_CEIdHelpersTest(void) {
    CE_Id id;
    // Build component id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)5, 0, 1234, &id));
    TEST_ASSERT_TRUE(CE_Id_isComponent(id));
    TEST_ASSERT_FALSE(CE_Id_isEntity(id));
    TEST_ASSERT_EQUAL_UINT32(1234, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT8(5, CE_Id_getComponentTypeId(id));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(id));

    // Build direct entity id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 7, 4321, &id));
    TEST_ASSERT_TRUE(CE_Id_isEntity(id));
    TEST_ASSERT_FALSE(CE_Id_isComponent(id));
    TEST_ASSERT_FALSE(CE_Id_isRelationship(id));
    TEST_ASSERT_EQUAL_UINT32(4321, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT32(7, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(id));

    // Build relationship entity id (generation unused for relationships)
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)9, 0, 555, &id));
    TEST_ASSERT_TRUE(CE_Id_isRelationship(id));
    TEST_ASSERT_FALSE(CE_Id_isEntity(id));
    TEST_ASSERT_EQUAL_UINT32(555, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT32(0, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(9, CE_Id_getRelationshipTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(id));

    // Setters enforcement
    CE_Id original = id;
    // Invalid: generation not allowed for relationships
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setGeneration(&id, 1));
    TEST_ASSERT_EQUAL_UINT32(original, id);
    // Invalid unique (>16 bits)
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setUniqueId(&id, 0x1FFFF));
    TEST_ASSERT_EQUAL_UINT32(original, id);

    // Valid modifications
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setUniqueId(&id, 123));
    TEST_ASSERT_EQUAL_UINT32(123, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setRelationshipTypeId(&id, (CE_TypeId)12));
    TEST_ASSERT_EQUAL_UINT8(12, CE_Id_getRelationshipTypeId(id));

    // Comparison tests
    CE_Id compA, compB, entA, entB, relA, relB;
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)2, 0, 100, &compA));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)7, 0, 100, &compB));
    TEST_ASSERT_TRUE(CE_Id_compare(compA, compB)); // same unique id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)2, 0, 101, &compB));
    TEST_ASSERT_FALSE(CE_Id_compare(compA, compB));

    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 5, 42, &entA));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 5, 42, &entB));
    TEST_ASSERT_TRUE(CE_Id_compare(entA, entB));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 6, 42, &entB));
    TEST_ASSERT_FALSE(CE_Id_compare(entA, entB));

    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)3, 0, 77, &relA));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)4, 0, 77, &relB));
    TEST_ASSERT_TRUE(CE_Id_compare(relA, relB)); // type ignored, generation unused
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)3, 0, 78, &relB));
    TEST_ASSERT_FALSE(CE_Id_compare(relA, relB));

    // Constructor failures set invalid id
    CE_Id out;
    //// Invalid kind
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_INVALID_KIND, (CE_TypeId)0, 0, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);
    //// Invalid kind: CE_ID_KIND_COUNT
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_KIND_COUNT, (CE_TypeId)0, 0, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);
    //// Invalid generation for entity reference
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 0xFF, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);
    //// Invalid unique id for entity reference
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 0, 0x1FFFF, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);

    // Constructor corner cases
    //// Generation is ignored for components
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)0x1, 0xFF, 10, &out));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, out);
    //// Invalid component reference with invalid type
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, CE_INVALID_TYPE_ID, 0x1, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);

    // Invalid tests
    CE_Id invalidId = CE_INVALID_ID;
    TEST_ASSERT_EQUAL_INT(CE_ID_INVALID_KIND, CE_Id_getKind(invalidId));
    TEST_ASSERT_EQUAL_INT(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(invalidId));
    TEST_ASSERT_EQUAL_INT(0, CE_Id_getGeneration(invalidId));
    TEST_ASSERT_EQUAL_INT(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(invalidId));
    TEST_ASSERT_EQUAL_INT(0, CE_Id_getUniqueId(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isComponent(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isEntity(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isRelationship(invalidId));
}

static void test_ComponentStorageTest(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result;
    CE_Id id;
    CE_Id anotherId;
    void *componentData = NULL;

    CE_ECS_ComponentStaticData* debugDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    CE_ECS_ComponentStorage* debugStorage = context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT];

    // Create the first component
    result = CE_ECS_MainStorage_createComponent(&context.m_storage, debugDesc, &id, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_EQUAL_size_t(1, debugStorage->m_count);
    if (componentData) {
        CE_CORE_DEBUG_COMPONENT_StorageType *debugComponentPtr = (CE_CORE_DEBUG_COMPONENT_StorageType*)componentData;
        TEST_ASSERT_FALSE(debugComponentPtr->m_enabled);
        debugComponentPtr->m_enabled = true; // Force change a value to test retrieval
    }

    // Check id fields
    TEST_ASSERT_EQUAL_UINT32(CE_ID_COMPONENT_REFERENCE_KIND, CE_Id_getKind(id));
    TEST_ASSERT_EQUAL_UINT32(CE_CORE_DEBUG_COMPONENT, CE_Id_getComponentTypeId(id));

    // Create another component to muddle things up a bit
    result = CE_ECS_MainStorage_createComponent(&context.m_storage, debugDesc, &anotherId, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_EQUAL_size_t(2, debugStorage->m_count);
    
    // Verify the first component is correct
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, id);
    TEST_ASSERT_NOT_NULL(componentData);
    if (componentData) {
        CE_CORE_DEBUG_COMPONENT_StorageType *debugComponentPtr = (CE_CORE_DEBUG_COMPONENT_StorageType*)componentData;
        TEST_ASSERT_TRUE(debugComponentPtr->m_enabled);
    }

    // Verify the second component is correct
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, anotherId);
    TEST_ASSERT_NOT_NULL(componentData);
    if (componentData) {
        CE_CORE_DEBUG_COMPONENT_StorageType *debugComponentPtr = (CE_CORE_DEBUG_COMPONENT_StorageType*)componentData;
        TEST_ASSERT_FALSE(debugComponentPtr->m_enabled);
    }

    // Test destroy the second component
    result = CE_ECS_MainStorage_destroyComponent(&context.m_storage, debugDesc, anotherId, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_size_t(1, debugStorage->m_count); 
    
    // Verify the first component is still intact
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, id);
    TEST_ASSERT_NOT_NULL(componentData);
    if (componentData) {
        CE_CORE_DEBUG_COMPONENT_StorageType *debugComponentPtr = (CE_CORE_DEBUG_COMPONENT_StorageType*)componentData;
        TEST_ASSERT_TRUE(debugComponentPtr->m_enabled);
    }

    // Verify the second component has been destroyed
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, anotherId);
    TEST_ASSERT_NULL(componentData);

    // Destroy the first component
    result = CE_ECS_MainStorage_destroyComponent(&context.m_storage, debugDesc, id, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_size_t(0, debugStorage->m_count);
    
    // Verify the first component has been destroyed
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, id);
    TEST_ASSERT_NULL(componentData);
}

static void test_CE_Bitset_InitTest(void) {
    CE_Bitset bitset;
    
    // Test valid initialization
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_init(&bitset, 64));
    TEST_ASSERT_EQUAL_size_t(64, bitset.m_size);
    
    // Verify all bits are cleared
    for (size_t i = 0; i < 64; i++) {
        TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, i));
    }
    
    // Test boundary: max size
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_init(&bitset, CE_BITSET_MAX_BITS));
    TEST_ASSERT_EQUAL_size_t(CE_BITSET_MAX_BITS, bitset.m_size);
    
    // Test boundary: size of 1
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_init(&bitset, 1));
    TEST_ASSERT_EQUAL_size_t(1, bitset.m_size);
    
    // Test error: size exceeds max
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_init(&bitset, CE_BITSET_MAX_BITS + 1));

    // Test error: zero size
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_init(&bitset, 0));
}

static void test_CE_Bitset_SetBitTest(void) {
    CE_Bitset bitset;
    
    CE_Bitset_init(&bitset, 64);
    
    // Set individual bits
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, 0));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 0));
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, 5));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 5));
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, 63));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 63));
    
    // Verify other bits remain clear
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 1));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 4));
    
    // Test setting same bit twice (should still be set)
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, 5));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 5));
    
    // Test boundary: last valid index
    CE_Bitset_init(&bitset, 100);
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, 99));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 99));
    
    // Test error: index out of bounds
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_setBit(&bitset, 100));
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_setBit(&bitset, 200));
}

static void test_CE_Bitset_ClearBitTest(void) {
    CE_Bitset bitset;
    
    CE_Bitset_init(&bitset, 64);
    
    // Set some bits then clear them
    CE_Bitset_setBit(&bitset, 0);
    CE_Bitset_setBit(&bitset, 5);
    CE_Bitset_setBit(&bitset, 10);
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, 5));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 5));
    
    // Verify other bits remain set
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 0));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 10));
    
    // Clear remaining bits
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, 0));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 0));
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, 10));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 10));
    
    // Test clearing already clear bit (should remain clear)
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, 5));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 5));
    
    // Test error: index out of bounds
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_clearBit(&bitset, 64));
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_clearBit(&bitset, 1000));

    // Test clearing all bits
    CE_Bitset_setBit(&bitset, 0);
    CE_Bitset_setBit(&bitset, 5);
    CE_Bitset_setBit(&bitset, 10);
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clear(&bitset));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 0));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 5));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 10));
}

static void test_CE_Bitset_ToggleBitTest(void) {
    CE_Bitset bitset;
    
    CE_Bitset_init(&bitset, 64);
    
    // Toggle bit from clear to set
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 0));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 0));
    
    // Toggle bit from set to clear
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 0));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 0));
    
    // Toggle multiple times
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 15));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 15));
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 15));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 15));
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 15));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 15));
    
    // Test across int32 boundaries
    CE_Bitset_init(&bitset, CE_BITSET_MAX_BITS);
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 31));   // Last bit of int32 0
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 32));   // First bit of int32 1
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 63));  // Last bit of int32 1
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_toggleBit(&bitset, 64));  // First bit of int32 2
    
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 31));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 32));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 63));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 64));
    
    // Test error: index out of bounds
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_toggleBit(&bitset, CE_BITSET_MAX_BITS));
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Bitset_toggleBit(&bitset, 400));
}

static void test_CE_Bitset_IsBitSetTest(void) {
    CE_Bitset bitset;
    
    CE_Bitset_init(&bitset, 64);
    
    // Check all bits are initially clear
    for (size_t i = 0; i < 64; i++) {
        TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, i));
    }
    
    // Set some bits and verify
    CE_Bitset_setBit(&bitset, 3);
    CE_Bitset_setBit(&bitset, 17);
    CE_Bitset_setBit(&bitset, 42);
    
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 3));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 17));
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, 42));
    
    // Verify adjacent bits are clear
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 2));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 4));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 16));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 18));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 41));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 43));
    
    // Test error: index out of bounds returns false
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 64));
    TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, 500));
}

static void test_CE_Bitset_ByteBoundariesTest(void) {
    CE_Bitset bitset;
    
    // Test operations across byte boundaries
    CE_Bitset_init(&bitset, CE_BITSET_MAX_BITS);
    
    // Set bits at byte boundaries
    for (size_t i = 0; i < CE_BITSET_MAX_BITS/32; i++) {
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, i * 32));  // First bit of each int32
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, i * 32 + 31));  // Last bit of each int32
    }
    
    // Verify set bits
    for (size_t i = 0; i < CE_BITSET_MAX_BITS/32; i++) {
        TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, i * 32));
        TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, i * 32 + 31));
        
        // Verify middle bits are clear
        if (i * 32 + 3 < CE_BITSET_MAX_BITS) {
            TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, i * 32 + 3));
        }
    }
    
    // Clear all boundary bits
    for (size_t i = 0; i < CE_BITSET_MAX_BITS/32; i++) {
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, i * 32));
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, i * 32 + 31));
    }
    
    // Verify all cleared
    for (size_t i = 0; i < CE_BITSET_MAX_BITS; i++) {
        TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, i));
    }
}

static void test_CE_Bitset_AllBitsTest(void) {
    CE_Bitset bitset;
    
    CE_Bitset_init(&bitset, CE_BITSET_MAX_BITS);
    
    // Set all bits
    for (size_t i = 0; i < CE_BITSET_MAX_BITS; i++) {
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_setBit(&bitset, i));
    }
    
    // Verify all bits are set
    for (size_t i = 0; i < CE_BITSET_MAX_BITS; i++) {
        TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&bitset, i));
    }
    
    // Clear all bits
    for (size_t i = 0; i < CE_BITSET_MAX_BITS; i++) {
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_Bitset_clearBit(&bitset, i));
    }
    
    // Verify all bits are clear
    for (size_t i = 0; i < CE_BITSET_MAX_BITS; i++) {
        TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&bitset, i));
    }
}

static void test_CE_EntityConstructionTest(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entityId = CE_INVALID_ID;
    CE_ECS_EntityData* entityData = NULL;

    // Construct one entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &entityId, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entityId);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT32(CE_ID_ENTITY_REFERENCE_KIND, CE_Id_getKind(entityId));
    TEST_ASSERT_EQUAL_UINT32(0, CE_Id_getGeneration(entityId));

    TEST_ASSERT_EQUAL_size_t(1, context.m_storage.m_entityStorage.m_count);
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_getEntityData(&context.m_storage, entityId, &entityData, &errorCode));
    TEST_ASSERT_NOT_NULL(entityData);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    TEST_ASSERT_TRUE(entityData->m_isValid);
    TEST_ASSERT_EQUAL_UINT32(entityId, entityData->m_entityId);
    const size_t componentCount = cc_size(&entityData->m_components);
    const size_t relationshipCount = cc_size(&entityData->m_relationships);
    TEST_ASSERT_EQUAL_size_t(0, componentCount);
    TEST_ASSERT_EQUAL_size_t(0, relationshipCount);

    // Check components and relationships are empty
    for (int i = 0; i < CE_COMPONENT_TYPES_COUNT; i++) {
        TEST_ASSERT_FALSE(CE_Bitset_isBitSet(&entityData->m_entityComponentBitset, i));
    }

    // Delete entity
    entityData = NULL;
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_destroyEntity(&context.m_storage, entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    TEST_ASSERT_EQUAL_size_t(0, context.m_storage.m_entityStorage.m_count);  
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_getEntityData(&context.m_storage, entityId, &entityData, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_ENTITY_NOT_FOUND, errorCode);

    // Use up all entity IDs
    for (CE_Id i = 0; i < CE_MAX_ENTITIES; i++) {
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &entityId, &errorCode));
        TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    }

    // Attempt to create one more entity should fail
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_createEntity(&context.m_storage, &entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_MAX_ENTITIES_REACHED, errorCode);

    // Delete one and create one
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_destroyEntity(&context.m_storage, entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    CE_Id newEntityId; // Save to a new variable
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &newEntityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify generation count has incremented
    TEST_ASSERT_EQUAL_UINT32(1, CE_Id_getGeneration(newEntityId));
    
    // entityID and newEntityId should have the same index but different generations
    TEST_ASSERT_EQUAL_UINT32(CE_Id_getUniqueId(entityId), CE_Id_getUniqueId(newEntityId));
    TEST_ASSERT_NOT_EQUAL(CE_Id_getGeneration(entityId), CE_Id_getGeneration(newEntityId));

    // Attempting to use old entityId should fail
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_getEntityData(&context.m_storage, entityId, &entityData, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_STALE_ENTITY_ID, errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_destroyEntity(&context.m_storage, entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_STALE_ENTITY_ID, errorCode);

    // Verify newEntityId is valid
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_getEntityData(&context.m_storage, newEntityId, &entityData, &errorCode));
    TEST_ASSERT_NOT_NULL(entityData);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_TRUE(entityData->m_isValid);
    TEST_ASSERT_EQUAL_UINT32(newEntityId, entityData->m_entityId);
}

void test_EntityComponentCreationTest(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id entity_2 = CE_INVALID_ID;
    CE_ECS_EntityData* entityData = NULL;
    CE_Id componentId_1 = CE_INVALID_ID;
    CE_Id componentId_2 = CE_INVALID_ID;
    CE_Id foundComponentId = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;
    CE_SPRITE_COMPONENT_StorageType* spriteComponentData = NULL;

    // Construct one entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to entity_1
    result = CE_ECS_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    componentData->m_enabled = true;
    componentData->m_debugValue = 42;

    // Test component access
    componentData = NULL;
    result = CE_ECS_GetComponent(&context, entity_1, componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_TRUE(componentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(42, componentData->m_debugValue);

    // Add a second component
    result = CE_ECS_AddComponent(&context, entity_1, CE_SPRITE_COMPONENT, &componentId_2, &spriteComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(spriteComponentData);

    // Test first component access again
    componentData = NULL;
    result = CE_ECS_GetComponent(&context, entity_1, componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_TRUE(componentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(42, componentData->m_debugValue);

    // Test second component access using FindFirstComponent
    spriteComponentData = NULL;
    result = CE_ECS_FindFirstComponent(&context, entity_1, CE_SPRITE_COMPONENT, &foundComponentId, &spriteComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(spriteComponentData);
    TEST_ASSERT_EQUAL_UINT32(componentId_2, foundComponentId);

    // Test FindAllComponents
    CE_Id foundComponents[10];
    size_t foundCount = 0;
    result = CE_ECS_FindAllComponents(&context, entity_1, CE_SPRITE_COMPONENT, foundComponents, 10, &foundCount, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT32(1, foundCount);
    TEST_ASSERT_EQUAL_UINT32(componentId_2, foundComponents[0]);
}

void test_EntityComponentDeletionTest(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id entity_2 = CE_INVALID_ID;
    CE_Id componentId_1 = CE_INVALID_ID;
    CE_Id componentId_2 = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData_2 = NULL;

    // Create entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to entity
    result = CE_ECS_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    // Create a second entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_createEntity(&context.m_storage, &entity_2, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_2);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to the second entity
    result = CE_ECS_AddComponent(&context, entity_2, CE_CORE_DEBUG_COMPONENT, &componentId_2, &componentData_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData_2);

    // Set some data
    componentData->m_enabled = true;
    componentData->m_debugValue = 42;
    componentData_2->m_enabled = true;
    componentData_2->m_debugValue = 84;

    // Delete first component
    result = CE_ECS_RemoveComponent(&context, entity_1, componentId_1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify first component has been deleted
    CE_CORE_DEBUG_COMPONENT_StorageType* deletedComponentData = NULL;
    result = CE_ECS_GetComponent(&context, entity_1, componentId_1, &deletedComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY, errorCode);
    TEST_ASSERT_NULL(deletedComponentData);

    // Verify second component still exists and has correct data
    CE_CORE_DEBUG_COMPONENT_StorageType* verifyComponentData = NULL;
    result = CE_ECS_GetComponent(&context, entity_2, componentId_2, &verifyComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(verifyComponentData);
    TEST_ASSERT_TRUE(verifyComponentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(84, verifyComponentData->m_debugValue);

    // Delete second component
    result = CE_ECS_RemoveComponent(&context, entity_2, componentId_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify second component has been deleted
    CE_CORE_DEBUG_COMPONENT_StorageType* deletedComponentData_2 = NULL;
    result = CE_ECS_GetComponent(&context, entity_2, componentId_2, &deletedComponentData_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY, errorCode);
    TEST_ASSERT_NULL(deletedComponentData_2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ECSContextSetupTest);
    RUN_TEST(test_CEIdHelpersTest);
    RUN_TEST(test_ComponentStorageTest);
    RUN_TEST(test_CE_Bitset_InitTest);
    RUN_TEST(test_CE_Bitset_SetBitTest);
    RUN_TEST(test_CE_Bitset_ClearBitTest);
    RUN_TEST(test_CE_Bitset_ToggleBitTest);
    RUN_TEST(test_CE_Bitset_IsBitSetTest);
    RUN_TEST(test_CE_Bitset_ByteBoundariesTest);
    RUN_TEST(test_CE_Bitset_AllBitsTest);
    RUN_TEST(test_CE_EntityConstructionTest);
    RUN_TEST(test_EntityComponentCreationTest);
    RUN_TEST(test_EntityComponentDeletionTest);
    return UNITY_END();
}
