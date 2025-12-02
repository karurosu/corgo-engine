#define CE_CORE_TEST_MODE
#include "unity.h"
#include "ecs/ecs.h"


void setUp(void) {
    // Set up test fixtures
}

void tearDown(void) {
    // Clean up test fixtures
}

void ECSContextSetupTest(void) {
    CE_ECS_Context context;
    CE_ECS_Init(&context);
    
    // Verify we have exactly 1 component type (CE_CORE_DEBUG_COMPONENT)
    TEST_ASSERT_EQUAL_UINT32(1, CE_COMPONENT_TYPE_COUNT);
    
    // Verify the debug component descriptor is valid
    CE_ComponentStaticData* debugDesc = &context.componentDefinitions[CE_CORE_DEBUG_COMPONENT];
    TEST_ASSERT_TRUE(debugDesc->isValid);
    TEST_ASSERT_EQUAL_UINT8(CE_CORE_DEBUG_COMPONENT, debugDesc->type);
    TEST_ASSERT_EQUAL_UINT32(0, debugDesc->uid);
    TEST_ASSERT_EQUAL_UINT64((uint64_t)1 << CE_CORE_DEBUG_COMPONENT, debugDesc->bitmask);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(ECSContextSetupTest);
    return UNITY_END();
}
