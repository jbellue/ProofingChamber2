#include <unity.h>

// Include the header-only files we're testing
#define UNIT_TEST
#include "../src/OptionalBool.h"

// Mock interfaces for SafePtr testing
class MockInterface {
public:
    virtual ~MockInterface() = default;
    virtual int getValue() const = 0;
};

class MockImplementation : public MockInterface {
public:
    int getValue() const override { return 42; }
};

class MockNullObject : public MockInterface {
public:
    static MockNullObject& getInstance() {
        static MockNullObject instance;
        return instance;
    }
    int getValue() const override { return 0; }
private:
    MockNullObject() = default;
};

// Simplified SafePtr for testing (same pattern as the real one)
class TestSafePtr {
public:
    static MockInterface* resolve(MockInterface* ptr) {
        if (ptr == nullptr) {
            return &MockNullObject::getInstance();
        }
        return ptr;
    }

    template<typename T>
    static bool isNull(T* ptr) {
        return ptr == nullptr;
    }
};

void setUp(void) {
    // set up code here, to run before every test
}

void tearDown(void) {
    // clean up code here, to run after every test
}

// Test isNull function
void test_SafePtr_isNull_with_nullptr(void) {
    MockInterface* ptr = nullptr;
    TEST_ASSERT_TRUE(TestSafePtr::isNull(ptr));
}

void test_SafePtr_isNull_with_valid_pointer(void) {
    MockImplementation impl;
    MockInterface* ptr = &impl;
    TEST_ASSERT_FALSE(TestSafePtr::isNull(ptr));
}

// Test resolve function
void test_SafePtr_resolve_with_nullptr(void) {
    MockInterface* ptr = nullptr;
    MockInterface* resolved = TestSafePtr::resolve(ptr);
    
    TEST_ASSERT_NOT_NULL(resolved);
    TEST_ASSERT_EQUAL(0, resolved->getValue()); // Null object returns default value
}

void test_SafePtr_resolve_with_valid_pointer(void) {
    MockImplementation impl;
    MockInterface* ptr = &impl;
    MockInterface* resolved = TestSafePtr::resolve(ptr);
    
    TEST_ASSERT_NOT_NULL(resolved);
    TEST_ASSERT_EQUAL(42, resolved->getValue()); // Real object returns actual value
    TEST_ASSERT_EQUAL(ptr, resolved); // Same pointer returned
}

// Test that null object singleton works
void test_SafePtr_null_object_singleton(void) {
    MockInterface* resolved1 = TestSafePtr::resolve(nullptr);
    MockInterface* resolved2 = TestSafePtr::resolve(nullptr);
    
    // Both should return the same singleton instance
    TEST_ASSERT_EQUAL(resolved1, resolved2);
}

// Test multiple resolve calls with different null pointers
void test_SafePtr_multiple_resolve_null(void) {
    MockInterface* ptr1 = nullptr;
    MockInterface* ptr2 = nullptr;
    
    MockInterface* resolved1 = TestSafePtr::resolve(ptr1);
    MockInterface* resolved2 = TestSafePtr::resolve(ptr2);
    
    TEST_ASSERT_NOT_NULL(resolved1);
    TEST_ASSERT_NOT_NULL(resolved2);
    TEST_ASSERT_EQUAL(resolved1, resolved2); // Same null object
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // isNull tests
    RUN_TEST(test_SafePtr_isNull_with_nullptr);
    RUN_TEST(test_SafePtr_isNull_with_valid_pointer);
    
    // resolve tests
    RUN_TEST(test_SafePtr_resolve_with_nullptr);
    RUN_TEST(test_SafePtr_resolve_with_valid_pointer);
    
    // Null object pattern tests
    RUN_TEST(test_SafePtr_null_object_singleton);
    RUN_TEST(test_SafePtr_multiple_resolve_null);
    
    return UNITY_END();
}
