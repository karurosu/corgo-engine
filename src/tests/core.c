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

    // Fake set up the scene graph
    CE_ECS_AccessGlobalComponentToVariable(&context, CE_ENGINE_SCENE_GRAPH_COMPONENT, sceneGraph);
    sceneGraph->m_rebuildZOrderCache = false;
    sceneGraph->m_needsRedraw = false;

}

void tearDown(void) {
    // Clean up test fixtures
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ECS_Cleanup(&context, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
}


void test_ECS_ContextSetup(void) {
    CE_ECS_SystemStaticData* sysDesc;
    CE_ECS_System_CacheList* cacheList;
    bool found = false;

    // Verify we have at least 1 component type (CE_CORE_DEBUG_COMPONENT)
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(1, CE_COMPONENT_TYPES_COUNT);
    
    // Verify the debug component descriptor is valid
    CE_ECS_ComponentStaticData  *compDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    TEST_ASSERT_TRUE(compDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_DEBUG_COMPONENT, compDesc->m_type);
    TEST_ASSERT_EQUAL_UINT32(0, compDesc->m_uid);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), compDesc->m_storageSizeOf);
    TEST_ASSERT_EQUAL_size_t(sizeof(CE_Core_DebugComponent), sizeof(CE_CORE_DEBUG_COMPONENT_StorageType));
    TEST_ASSERT_EQUAL_size_t(CE_DEFAULT_COMPONENT_CAPACITY, compDesc->m_initialCapacity);

    // Verify we have at least 4 system types (1 debug + 3 test systems)
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(4, CE_SYSTEM_TYPES_COUNT);

    // Verify the debug system descriptor is valid
    sysDesc = &context.m_systemDefinitions[CE_CORE_ENTITY_DEBUG_SYSTEM];
    TEST_ASSERT_TRUE(sysDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_ENTITY_DEBUG_SYSTEM, sysDesc->m_systemId);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_ORDER_AUTO, sysDesc->m_runOrder);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_PHASE_LATE, sysDesc->m_runPhase);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, sysDesc->m_runFrequency);
    TEST_ASSERT_NOT_NULL(sysDesc->m_runFunction);
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&sysDesc->m_requiredComponentBitset, CE_CORE_DEBUG_COMPONENT));

    cacheList = &context.m_systemRuntimeData.m_systemsByRunOrder[sysDesc->m_runOrder].m_frequency[sysDesc->m_runFrequency].m_phase[sysDesc->m_runPhase];
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(1, cc_size(&cacheList->m_systems));
    found = false;
    cc_for_each(&cacheList->m_systems, sysTypeIdPtr) {
        if (*sysTypeIdPtr == sysDesc->m_systemId) {
            found = true;
            break;
        }
    }

    // Verify test systems
    sysDesc = &context.m_systemDefinitions[CE_CORE_TEST_SYSTEM_DISPLAY];
    TEST_ASSERT_TRUE(sysDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_TEST_SYSTEM_DISPLAY, sysDesc->m_systemId);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_ORDER_AUTO, sysDesc->m_runOrder);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, sysDesc->m_runPhase);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, sysDesc->m_runFrequency);
    TEST_ASSERT_NOT_NULL(sysDesc->m_runFunction);
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&sysDesc->m_requiredComponentBitset, CE_CORE_DEBUG_COMPONENT));

    cacheList = &context.m_systemRuntimeData.m_systemsByRunOrder[sysDesc->m_runOrder].m_frequency[sysDesc->m_runFrequency].m_phase[sysDesc->m_runPhase];
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(1, cc_size(&cacheList->m_systems));
    found = false;
    cc_for_each(&cacheList->m_systems, sysTypeIdPtr) {
        if (*sysTypeIdPtr == sysDesc->m_systemId) {
            found = true;
            break;
        }
    }

    sysDesc = &context.m_systemDefinitions[CE_CORE_TEST_SYSTEM_HALF_DISPLAY];
    TEST_ASSERT_TRUE(sysDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_TEST_SYSTEM_HALF_DISPLAY, sysDesc->m_systemId);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_ORDER_AUTO, sysDesc->m_runOrder);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_PHASE_EARLY, sysDesc->m_runPhase);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY, sysDesc->m_runFrequency);
    TEST_ASSERT_NOT_NULL(sysDesc->m_runFunction);
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&sysDesc->m_requiredComponentBitset, CE_CORE_DEBUG_COMPONENT));

    cacheList = &context.m_systemRuntimeData.m_systemsByRunOrder[sysDesc->m_runOrder].m_frequency[sysDesc->m_runFrequency].m_phase[sysDesc->m_runPhase];
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(1, cc_size(&cacheList->m_systems));
    found = false;
    cc_for_each(&cacheList->m_systems, sysTypeIdPtr) {
        if (*sysTypeIdPtr == sysDesc->m_systemId) {
            found = true;
            break;
        }
    }

    sysDesc = &context.m_systemDefinitions[CE_CORE_TEST_SYSTEM_SECOND];
    TEST_ASSERT_TRUE(sysDesc->m_isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_TEST_SYSTEM_SECOND, sysDesc->m_systemId);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_ORDER_AUTO, sysDesc->m_runOrder);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_PHASE_LATE, sysDesc->m_runPhase);
    TEST_ASSERT_EQUAL_INT(CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND, sysDesc->m_runFrequency);
    TEST_ASSERT_NOT_NULL(sysDesc->m_runFunction);
    TEST_ASSERT_TRUE(CE_Bitset_isBitSet(&sysDesc->m_requiredComponentBitset, CE_CORE_DEBUG_COMPONENT));

    cacheList = &context.m_systemRuntimeData.m_systemsByRunOrder[sysDesc->m_runOrder].m_frequency[sysDesc->m_runFrequency].m_phase[sysDesc->m_runPhase];
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(1, cc_size(&cacheList->m_systems));
    found = false;
    cc_for_each(&cacheList->m_systems, sysTypeIdPtr) {
        if (*sysTypeIdPtr == sysDesc->m_systemId) {
            found = true;
            break;
        }
    }
}

static void test_CE_Id_Helpers(void) {
    CE_Id id;
    // Build component id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_COMPONENT_REFERENCE_KIND, (CE_TypeId)5, 0, 1234, &id));
    TEST_ASSERT_TRUE(CE_Id_isComponent(id));
    TEST_ASSERT_FALSE(CE_Id_isEntity(id));
    TEST_ASSERT_EQUAL_UINT16(1234, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT8(5, CE_Id_getComponentTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_ID, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(id));

    // Build direct entity id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 7, 4321, &id));
    TEST_ASSERT_TRUE(CE_Id_isEntity(id));
    TEST_ASSERT_FALSE(CE_Id_isComponent(id));
    TEST_ASSERT_FALSE(CE_Id_isRelationship(id));
    TEST_ASSERT_EQUAL_UINT16(4321, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT8(7, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(id));

    // Build relationship entity id
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)9, 3, 555, &id));
    TEST_ASSERT_TRUE(CE_Id_isRelationship(id));
    TEST_ASSERT_FALSE(CE_Id_isEntity(id));
    TEST_ASSERT_EQUAL_UINT16(555, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_UINT8(3, CE_Id_getGeneration(id));
    TEST_ASSERT_EQUAL_UINT8(9, CE_Id_getRelationshipTypeId(id));
    TEST_ASSERT_EQUAL_UINT8(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(id));

    CE_Id original = id;

    // Invalid unique (>16 bits)
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_Id_setUniqueId(&id, 0x1FFFF));
    TEST_ASSERT_EQUAL_UINT32(original, id);

    // Valid modifications
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setUniqueId(&id, 123));
    TEST_ASSERT_EQUAL_UINT16(123, CE_Id_getUniqueId(id));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setRelationshipTypeId(&id, (CE_TypeId)12));
    TEST_ASSERT_EQUAL_UINT8(12, CE_Id_getRelationshipTypeId(id));
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_setGeneration(&id, 5));
    TEST_ASSERT_EQUAL_UINT8(5, CE_Id_getGeneration(id));

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
    //// Passing in a type gets ignored for entity references
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0x1, 0x2, 10, &out));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, out);
    TEST_ASSERT_TRUE(CE_Id_isEntity(out));
    TEST_ASSERT_EQUAL_UINT16(10, CE_Id_getUniqueId(out));
    TEST_ASSERT_EQUAL_UINT8(2, CE_Id_getGeneration(out));


    // Invalid tests
    CE_Id invalidId = CE_INVALID_ID;
    TEST_ASSERT_EQUAL_INT(CE_ID_INVALID_KIND, CE_Id_getKind(invalidId));
    TEST_ASSERT_EQUAL_INT(CE_INVALID_TYPE_ID, CE_Id_getComponentTypeId(invalidId));
    TEST_ASSERT_EQUAL_UINT8(0, CE_Id_getGeneration(invalidId));
    TEST_ASSERT_EQUAL_INT(CE_INVALID_TYPE_ID, CE_Id_getRelationshipTypeId(invalidId));
    TEST_ASSERT_EQUAL_UINT16(0, CE_Id_getUniqueId(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isComponent(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isEntity(invalidId));
    TEST_ASSERT_FALSE(CE_Id_isRelationship(invalidId));

    // Relationship to entity reference conversion
    CE_Id relId;
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_RELATIONSHIP_KIND, (CE_TypeId)15, 4, 8888, &relId));
    CE_Id entRefId = CE_Id_relationshipToEntityReference(relId);
    TEST_ASSERT_TRUE(CE_Id_isEntity(entRefId));
    TEST_ASSERT_EQUAL_UINT16(8888, CE_Id_getUniqueId(entRefId));
    TEST_ASSERT_EQUAL_UINT8(4, CE_Id_getGeneration(entRefId));
    
    // Ensure we get the same entity as one constructed directly
    CE_Id sameEntityId;
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_Id_make(CE_ID_ENTITY_REFERENCE_KIND, (CE_TypeId)0, 4, 8888, &sameEntityId));
    TEST_ASSERT_EQUAL_UINT32(entRefId, sameEntityId);
}

static void test_ECS_ComponentStorage(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result;
    CE_Id id;
    CE_Id anotherId;
    void *componentData = NULL;

    CE_ECS_ComponentStaticData* debugDesc = &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    CE_ECS_ComponentStorage* debugStorage = context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT];

    // Create the first component
    result = CE_ECS_MainStorage_createComponent(&context.m_storage, &context, debugDesc, &id, &componentData, &errorCode);
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
    result = CE_ECS_MainStorage_createComponent(&context.m_storage, &context, debugDesc, &anotherId, &componentData, &errorCode);
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
    result = CE_ECS_MainStorage_destroyComponent(&context.m_storage, &context, debugDesc, anotherId, &errorCode);
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
    result = CE_ECS_MainStorage_destroyComponent(&context.m_storage, &context, debugDesc, id, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_size_t(0, debugStorage->m_count);
    
    // Verify the first component has been destroyed
    componentData = NULL;
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], debugDesc, id);
    TEST_ASSERT_NULL(componentData);
}

static void test_CE_Bitset_Init(void) {
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

static void test_CE_Bitset_SetBit(void) {
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

static void test_CE_Bitset_ClearBit(void) {
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

static void test_CE_Bitset_ToggleBit(void) {
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

static void test_CE_Bitset_IsBitSet(void) {
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

static void test_CE_Bitset_ByteBoundaries(void) {
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

static void test_CE_Bitset_AllBits(void) {
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

static void test_CE_EntityConstruction(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entityId = CE_INVALID_ID;
    CE_ECS_EntityData* entityData = NULL;

    // Construct one entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entityId, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entityId);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT32(CE_ID_ENTITY_REFERENCE_KIND, CE_Id_getKind(entityId));
    TEST_ASSERT_EQUAL_UINT8(0, CE_Id_getGeneration(entityId));
    TEST_ASSERT_EQUAL_size_t(1, context.m_storage.m_entityStorage.m_count);
    
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_getEntityData(&context.m_storage, entityId, &entityData, &errorCode));
    TEST_ASSERT_NOT_NULL(entityData);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    TEST_ASSERT_TRUE(CE_Entity_IsValid(&context, entityId));
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
        TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entityId, &errorCode));
        TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    }

    // Attempt to create one more entity should fail
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_CreateEntity(&context, &entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_MAX_ENTITIES_REACHED, errorCode);

    // Delete one and create one
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_DestroyEntity(&context, entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    CE_Id newEntityId; // Save to a new variable
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &newEntityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify generation count has incremented
    TEST_ASSERT_EQUAL_UINT8(1, CE_Id_getGeneration(newEntityId));
    
    // entityID and newEntityId should have the same index but different generations
    TEST_ASSERT_EQUAL_UINT16(CE_Id_getUniqueId(entityId), CE_Id_getUniqueId(newEntityId));
    TEST_ASSERT_NOT_EQUAL_UINT8(CE_Id_getGeneration(entityId), CE_Id_getGeneration(newEntityId));

    // Attempting to use old entityId should fail
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_getEntityData(&context.m_storage, entityId, &entityData, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_STALE_ENTITY_ID, errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, CE_ECS_MainStorage_destroyEntity(&context.m_storage, entityId, &errorCode));
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_STALE_ENTITY_ID, errorCode);

    // Verify newEntityId is valid
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_MainStorage_getEntityData(&context.m_storage, newEntityId, &entityData, &errorCode));
    TEST_ASSERT_NOT_NULL(entityData);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_TRUE(CE_Entity_IsValid(&context, newEntityId));
    TEST_ASSERT_EQUAL_UINT32(newEntityId, entityData->m_entityId);
}

void test_Entity_ComponentCreation(void) {
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
    size_t count = 0;

    // Construct one entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to entity_1
    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    componentData->m_enabled = true;
    componentData->m_testValue = 42;

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(1, count);

    TEST_ASSERT_TRUE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));

    // Test component access
    componentData = NULL;
    result = CE_Entity_GetComponent(&context, entity_1, componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_TRUE(componentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(42, componentData->m_testValue);

    // Add a second component
    result = CE_Entity_AddComponent(&context, entity_1, CE_SPRITE_COMPONENT, &componentId_2, &spriteComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(spriteComponentData);

    count =CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(2, count);

    // Test first component access again
    componentData = NULL;
    result = CE_Entity_GetComponent(&context, entity_1, componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);
    TEST_ASSERT_TRUE(componentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(42, componentData->m_testValue);

    // Test second component access using FindFirstComponent
    spriteComponentData = NULL;
    result = CE_Entity_FindFirstComponent(&context, entity_1, CE_SPRITE_COMPONENT, &foundComponentId, &spriteComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(spriteComponentData);
    TEST_ASSERT_EQUAL_UINT32(componentId_2, foundComponentId);
}

void test_Entity_ComponentDeletion(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id entity_2 = CE_INVALID_ID;
    CE_Id componentId_1 = CE_INVALID_ID;
    CE_Id componentId_2 = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData_2 = NULL;
    size_t count = 0;

    // Create entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to entity
    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(1, count);
    TEST_ASSERT_TRUE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));

    // Create a second entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_2, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_2);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to the second entity
    result = CE_Entity_AddComponent(&context, entity_2, CE_CORE_DEBUG_COMPONENT, &componentId_2, &componentData_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData_2);

    // Set some data
    componentData->m_enabled = true;
    componentData->m_testValue = 42;
    componentData_2->m_enabled = true;
    componentData_2->m_testValue = 84;

    // Delete first component
    result = CE_Entity_RemoveComponent(&context, entity_1, componentId_1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(0, count);
    TEST_ASSERT_FALSE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));

    // Verify first component has been deleted
    CE_CORE_DEBUG_COMPONENT_StorageType* deletedComponentData = NULL;
    result = CE_Entity_GetComponent(&context, entity_1, componentId_1, &deletedComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY, errorCode);
    TEST_ASSERT_NULL(deletedComponentData);

    // Attempt to delete again should fail
    result = CE_Entity_RemoveComponent(&context, entity_1, componentId_1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY, errorCode);

    // Verify second component still exists and has correct data
    CE_CORE_DEBUG_COMPONENT_StorageType* verifyComponentData = NULL;
    result = CE_Entity_GetComponent(&context, entity_2, componentId_2, &verifyComponentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(verifyComponentData);
    TEST_ASSERT_TRUE(verifyComponentData->m_enabled);
    TEST_ASSERT_EQUAL_INT(84, verifyComponentData->m_testValue);

    // Delete second component
    result = CE_Entity_RemoveComponent(&context, entity_2, componentId_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify second component has been deleted
    CE_CORE_DEBUG_COMPONENT_StorageType* deletedComponentData_2 = NULL;
    result = CE_Entity_GetComponent(&context, entity_2, componentId_2, &deletedComponentData_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_COMPONENT_NOT_FOUND_IN_ENTITY, errorCode);
    TEST_ASSERT_NULL(deletedComponentData_2);
}

void test_Entity_Deletion(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id componentId_1 = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;
    
    // Create entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // create a component for the entity
    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    // Delete entity
    result = CE_ECS_DestroyEntity(&context, entity_1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Attempt to get the component of the deleted entity
    componentData = NULL;
    result = CE_Entity_GetComponent(&context, entity_1, componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_ENTITY_NOT_FOUND, errorCode);
    TEST_ASSERT_NULL(componentData);

    // Attempt to access component directly fails too
    componentData = CE_ECS_ComponentStorage_getComponentDataPointerById(context.m_storage.m_componentTypeStorage[CE_CORE_DEBUG_COMPONENT], &context.m_componentDefinitions[CE_CORE_DEBUG_COMPONENT], componentId_1);
    TEST_ASSERT_NULL(componentData);

    // IsValid should return false
    TEST_ASSERT_FALSE(CE_Entity_IsValid(&context, entity_1));
}

void test_Entity_MultipleComponents(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id componentId_1 = CE_INVALID_ID;
    CE_Id componentId_2 = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;
    size_t count = 0;
    
    // Create entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create two debug components
    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_1, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId_2, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(2, count);
    TEST_ASSERT_TRUE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));

    // Test FindAllComponents
    CE_Id foundComponents[10];
    size_t foundCount = 0;
    result = CE_Entity_FindAllComponents(&context, entity_1, CE_CORE_DEBUG_COMPONENT, foundComponents, 10, &foundCount, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT32(2, foundCount);
    TEST_ASSERT_TRUE(foundComponents[0] == componentId_1 || foundComponents[1] == componentId_1);
    TEST_ASSERT_TRUE(foundComponents[0] == componentId_2 || foundComponents[1] == componentId_2);
    
    // Test FindAllComponentsIter
    CE_Id_Set *components = NULL;
    result = CE_Entity_GetAllComponentsIter(&context, entity_1, &components, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(components);
    TEST_ASSERT_EQUAL_UINT32(2, cc_size(components));
    
    // Delete one component
    result = CE_Entity_RemoveComponent(&context, entity_1, componentId_1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(1, count);
    TEST_ASSERT_TRUE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));

    // Delete the remaining component
    result = CE_Entity_RemoveComponent(&context, entity_1, componentId_2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    count = CE_Entity_GetComponentCount(&context, entity_1);
    TEST_ASSERT_EQUAL_UINT32(0, count);
    TEST_ASSERT_FALSE(CE_Entity_HasComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT));
}

void test_ECS_Relationships(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id parentEntity = CE_INVALID_ID;
    CE_Id childEntity1 = CE_INVALID_ID;
    CE_Id childEntity2 = CE_INVALID_ID;

    // Check that reciprocal data is correct
    TEST_ASSERT_EQUAL_UINT32(CE_RELATIONSHIP_PARENT, CE_RELATIONSHIPS_RECIPROCALS[CE_RELATIONSHIP_CHILD]);
    TEST_ASSERT_EQUAL_UINT32(CE_RELATIONSHIP_CHILD, CE_RELATIONSHIPS_RECIPROCALS[CE_RELATIONSHIP_PARENT]);

    // Create parent entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &parentEntity, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, parentEntity);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create child entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &childEntity1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, childEntity1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create a second child entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &childEntity2, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, childEntity2);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add relationship from parent to child
    result = CE_Entity_AddRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify relationship exists
    TEST_ASSERT_EQUAL_UINT32(1, CE_Entity_GetRelationshipCount(&context, parentEntity));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD));
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_PARENT)); // Check parent didn't get added mistakenly

    // Check has specific relationship
    bool exists = false;
    result = CE_Entity_HasSpecificRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity1, &exists, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_TRUE(exists);

    // Ensure relationship to entity 2 does not exist yet
    result = CE_Entity_HasSpecificRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity2, &exists, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_FALSE(exists);
    
    // Check reciprocal relationship from child to parent
    TEST_ASSERT_EQUAL_UINT32(1, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT));

    // Add a second relationship from parent to another child
    result = CE_Entity_AddRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Check all relationships
    TEST_ASSERT_EQUAL_UINT32(2, CE_Entity_GetRelationshipCount(&context, parentEntity));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD));
    TEST_ASSERT_EQUAL_UINT32(1, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT));
    TEST_ASSERT_EQUAL_UINT32(1, CE_Entity_GetRelationshipCount(&context, childEntity2));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, childEntity2, CE_RELATIONSHIP_PARENT));

    CE_Id found = CE_INVALID_ID;
    result = CE_Entity_FindFirstRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT, &found, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT16(CE_Id_getUniqueId(parentEntity), CE_Id_getUniqueId(found));
    result = CE_Entity_FindFirstRelationship(&context, childEntity2, CE_RELATIONSHIP_PARENT, &found, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_UINT16(CE_Id_getUniqueId(parentEntity), CE_Id_getUniqueId(found));

    CE_Id foundChildren[3];
    size_t foundCount = 0;
    result = CE_Entity_FindAllRelationships(&context, parentEntity, CE_RELATIONSHIP_CHILD, foundChildren, 3, &foundCount, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_size_t(2, foundCount);
    for (size_t i = 0; i < foundCount; i++) {
        const uint16_t childId = CE_Id_getUniqueId(foundChildren[i]);
        TEST_ASSERT_TRUE(childId == CE_Id_getUniqueId(childEntity1) || childId == CE_Id_getUniqueId(childEntity2));
    }

    // Remove second relationship
    result = CE_Entity_RemoveRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_size_t(1, CE_Entity_GetRelationshipCount(&context, parentEntity));
    TEST_ASSERT_EQUAL_size_t(0, CE_Entity_GetRelationshipCount(&context, childEntity2));
    TEST_ASSERT_EQUAL_size_t(1, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD)); // Still has one child
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT)); // Still has parent
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, childEntity2, CE_RELATIONSHIP_PARENT)); // No longer has parent

    // Now try again and it should fail
    result = CE_Entity_RemoveRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity2, &errorCode);
    TEST_ASSERT_EQUAL_UINT32(CE_ERROR_CODE_ENTITY_DOES_NOT_HAVE_RELATIONSHIP, errorCode);
    TEST_ASSERT_EQUAL_INT(CE_ERROR, result);

    // Nothing should have changed
    TEST_ASSERT_EQUAL_size_t(1, CE_Entity_GetRelationshipCount(&context, parentEntity));
    TEST_ASSERT_EQUAL_size_t(0, CE_Entity_GetRelationshipCount(&context, childEntity2));
    TEST_ASSERT_EQUAL_size_t(1, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD)); // Still has one child
    TEST_ASSERT_TRUE(CE_Entity_HasRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT)); // Still has parent
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, childEntity2, CE_RELATIONSHIP_PARENT)); // No longer has parent

    // Remove first relationship from reciprocal relationship
    result = CE_Entity_RemoveRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT, parentEntity, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify all relationships removed
    TEST_ASSERT_EQUAL_size_t(0, CE_Entity_GetRelationshipCount(&context, parentEntity));
    TEST_ASSERT_EQUAL_size_t(0, CE_Entity_GetRelationshipCount(&context, childEntity2));
    TEST_ASSERT_EQUAL_size_t(0, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD)); // No longer has children
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, childEntity1, CE_RELATIONSHIP_PARENT)); // No longer has parent
    TEST_ASSERT_FALSE(CE_Entity_HasRelationship(&context, childEntity2, CE_RELATIONSHIP_PARENT)); // No longer has parent
}

void test_ECS_Clean_Relationships(void) {
    CE_ERROR_CODE errorCode;
    CE_Result result = CE_ERROR;
    CE_Id parentEntity = CE_INVALID_ID;
    CE_Id childEntity1 = CE_INVALID_ID;
    CE_Id childEntity2 = CE_INVALID_ID;

    // Create parent entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &parentEntity, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, parentEntity);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create child entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &childEntity1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, childEntity1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create a second child entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &childEntity2, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, childEntity2);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add relationship from parent to child
    result = CE_Entity_AddRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity1, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a second relationship from parent to another child
    result = CE_Entity_AddRelationship(&context, parentEntity, CE_RELATIONSHIP_CHILD, childEntity2, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Remove Parent
    result = CE_ECS_DestroyEntity(&context, parentEntity, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Verify all relationships removed
    TEST_ASSERT_EQUAL_UINT32(0, CE_Entity_GetRelationshipCount(&context, childEntity1));
    TEST_ASSERT_EQUAL_UINT32(0, CE_Entity_GetRelationshipCount(&context, childEntity2));
}

void test_ECS_tick(void) {
    CE_ERROR_CODE errorCode = CE_ERROR_CODE_NONE;
    CE_Result result = CE_ERROR;
    CE_Id entity_1 = CE_INVALID_ID;
    CE_Id entity_2 = CE_INVALID_ID;
    CE_CORE_DEBUG_COMPONENT_StorageType* componentData = NULL;

    // Create entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_1, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_1);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Create a second entity
    TEST_ASSERT_EQUAL_INT(CE_OK, CE_ECS_CreateEntity(&context, &entity_2, &errorCode));
    TEST_ASSERT_NOT_EQUAL_UINT32(CE_INVALID_ID, entity_2);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);

    // Add a debug component to entity 1
    CE_Id componentId = CE_INVALID_ID;
    result = CE_Entity_AddComponent(&context, entity_1, CE_CORE_DEBUG_COMPONENT, &componentId, &componentData, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_NOT_NULL(componentData);

    // Run immediate tick, should only run display systems for 1 entity
    result = CE_ECS_Tick(&context, 0.0f, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_INT(1, componentData->m_testValue);
    TEST_ASSERT_TRUE(componentData->m_ticked_display);
    TEST_ASSERT_FALSE(componentData->m_ticked_half);
    TEST_ASSERT_FALSE(componentData->m_ticked_second);

    // Reset test variables
    componentData->m_ticked_display = false;
    componentData->m_ticked_half = false;
    componentData->m_ticked_second = false;
    componentData->m_testValue = 0;

    // Now run with a delta of 0.6, triggering display and half tick systems
    result = CE_ECS_Tick(&context, 0.6f, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_INT(2, componentData->m_testValue);
    TEST_ASSERT_TRUE(componentData->m_ticked_display);
    TEST_ASSERT_TRUE(componentData->m_ticked_half);
    TEST_ASSERT_FALSE(componentData->m_ticked_second);

    // Reset test variables
    componentData->m_ticked_display = false;
    componentData->m_ticked_half = false;
    componentData->m_ticked_second = false;
    componentData->m_testValue = 0;

    // Tick with a delta of 0.1, triggering only display tick
    result = CE_ECS_Tick(&context, 0.1f, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_INT(1, componentData->m_testValue);
    TEST_ASSERT_TRUE(componentData->m_ticked_display);
    TEST_ASSERT_FALSE(componentData->m_ticked_half);
    TEST_ASSERT_FALSE(componentData->m_ticked_second);

    // Reset test variables
    componentData->m_ticked_display = false;
    componentData->m_ticked_half = false;
    componentData->m_ticked_second = false;
    componentData->m_testValue = 0;

    // Tick with a delta of 0.4, triggering all ticks
    result = CE_ECS_Tick(&context, 0.4f, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_INT(3, componentData->m_testValue);
    TEST_ASSERT_TRUE(componentData->m_ticked_display);
    TEST_ASSERT_TRUE(componentData->m_ticked_half);
    TEST_ASSERT_TRUE(componentData->m_ticked_second);
}

void test_ECS_GlobalComponents(void) {
    CE_ERROR_CODE errorCode = CE_ERROR_CODE_COUNT;
    CE_Result result = CE_ERROR;

    // Get global component
    CE_ECS_AccessGlobalComponentToVariable(&context, CE_CORE_GLOBAL_DEBUG_COMPONENT, globalDebugComponentPtr);
    TEST_ASSERT_NOT_NULL(globalDebugComponentPtr);
    TEST_ASSERT_FALSE(globalDebugComponentPtr->m_enabled);
    TEST_ASSERT_EQUAL_INT(0, globalDebugComponentPtr->m_testValue);

    // Enable debug mode
    globalDebugComponentPtr->m_enabled = true;

    // Tick with a delta of 1.1 seconds, which should trigger the global system
    result = CE_ECS_Tick(&context, 1.1f, &errorCode);
    TEST_ASSERT_EQUAL_INT(CE_OK, result);
    TEST_ASSERT_EQUAL_INT(CE_ERROR_CODE_NONE, errorCode);
    TEST_ASSERT_EQUAL_INT(1, globalDebugComponentPtr->m_testValue);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_CE_Id_Helpers);

    RUN_TEST(test_CE_Bitset_Init);
    RUN_TEST(test_CE_Bitset_SetBit);
    RUN_TEST(test_CE_Bitset_ClearBit);
    RUN_TEST(test_CE_Bitset_ToggleBit);
    RUN_TEST(test_CE_Bitset_IsBitSet);
    RUN_TEST(test_CE_Bitset_ByteBoundaries);
    RUN_TEST(test_CE_Bitset_AllBits);
    RUN_TEST(test_CE_EntityConstruction);

    RUN_TEST(test_ECS_ContextSetup);
    RUN_TEST(test_ECS_ComponentStorage);
    RUN_TEST(test_ECS_Relationships);
    RUN_TEST(test_ECS_Clean_Relationships);
    
    RUN_TEST(test_Entity_ComponentCreation);
    RUN_TEST(test_Entity_ComponentDeletion);
    RUN_TEST(test_Entity_Deletion);
    RUN_TEST(test_Entity_MultipleComponents);

    RUN_TEST(test_ECS_tick);
    RUN_TEST(test_ECS_GlobalComponents);
        
    return UNITY_END();
}
