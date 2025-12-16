#include "unity.h"
#include "ecs/types.h"
#include "ecs/ecs.h"

void setUp(void) {
    // Set up test fixtures
}

void tearDown(void) {
    // Clean up test fixtures
}

void ECSContextSetupTest(void) {
    CE_ECS_Context context;
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ECS_Init(&context, &errorCode);

    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    
    // Verify we have exactly 1 component type (CE_CORE_DEBUG_COMPONENT)
    TEST_ASSERT_EQUAL_UINT32(1, CE_COMPONENT_TYPES_COUNT);
    
    // Verify the debug component descriptor is valid
    CE_ECS_ComponentStaticData* debugDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    TEST_ASSERT_TRUE(debugDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_DEBUG_COMPONENT, debugDesc->m_type);
    TEST_ASSERT_EQUAL_UINT32(0, debugDesc->m_uid);
    TEST_ASSERT_EQUAL_UINT64((uint64_t)1 << CE_CORE_DEBUG_COMPONENT, debugDesc->m_bitmask);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), debugDesc->m_storageSizeOf);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), sizeof(CE_CORE_DEBUG_COMPONENT_StorageType));
    TEST_ASSERT_EQUAL_size_t(CE_DEFAULT_COMPONENT_CAPACITY, debugDesc->m_initialCapacity);

    // Verify cleanup
	result = CE_ECS_Cleanup(&context, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
	TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
	TEST_ASSERT_NULL(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT]);
}

static void CEIdHelpersTest(void) {
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

    // Build relationship entity id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)9, 15, 555, &id));
    TEST_ASSERT_TRUE(CE_Id_isRelationship(id));
    TEST_ASSERT_TRUE(CE_Id_isEntity(id));
    TEST_ASSERT_EQUAL_UINT32(555, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT32(15, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(9, CE_Id_getRelationshipTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(id));

    // Setters enforcement
    CE_Id original = id;
    // Invalid type (>6 bits)
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setRelationshipTypeId(&id, (CE_TypeId)0xFF));
    TEST_ASSERT_EQUAL_UINT32(original, id);
    // Invalid generation (>6 bits)
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setGeneration(&id, 0xFF));
    TEST_ASSERT_EQUAL_UINT32(original, id);
    // Invalid unique (>16 bits)
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setUniqueId(&id, 0x1FFFF));
    TEST_ASSERT_EQUAL_UINT32(original, id);

    // Valid modifications
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setUniqueId(&id, 123));
    TEST_ASSERT_EQUAL_UINT32(123, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setGeneration(&id, 22));
    TEST_ASSERT_EQUAL_UINT32(22, CE_Id_getGeneration(id));
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

    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)3, 9, 77, &relA));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)4, 9, 77, &relB));
    TEST_ASSERT_TRUE(CE_Id_compare(relA, relB)); // type ignored
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)3, 10, 77, &relB));
    TEST_ASSERT_FALSE(CE_Id_compare(relA, relB));

    // Constructor failures set invalid id
    CE_Id out;
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)0xFF, 0, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 0xFF, 10, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 0, 0x1FFFF, &out));
    TEST_ASSERT_EQUAL_UINT32(CE_INVALID_ID, out);

    // Constructor corner cases
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)0x1, 0xFF, 10, &out));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, out);

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

static void ComponentStorageTest(void) {
    CE_ECS_Context context;
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ECS_Init(&context, &errorCode);
    CE_Id id;

    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    CE_ECS_ComponentStaticData* debugDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];

    CE_ECS_MainStorage_createComponent(&context.m_storage, debugDesc, &id, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(ECSContextSetupTest);
    RUN_TEST(CEIdHelpersTest);
    RUN_TEST(ComponentStorageTest);
    return UNITY_END();
}
