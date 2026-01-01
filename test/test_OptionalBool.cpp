#include <unity.h>
#include "../src/OptionalBool.h"

void setUp(void) {
    // set up code here, to run before every test
}

void tearDown(void) {
    // clean up code here, to run after every test
}

// Test constructor
void test_OptionalBool_constructor_default(void) {
    OptionalBool opt;
    TEST_ASSERT_EQUAL(OptionalBool::Unset, opt.state);
    TEST_ASSERT_FALSE(opt.isSet());
}

void test_OptionalBool_constructor_true(void) {
    OptionalBool opt(true);
    TEST_ASSERT_EQUAL(OptionalBool::True, opt.state);
    TEST_ASSERT_TRUE(opt.isSet());
    TEST_ASSERT_TRUE(opt.getValue());
}

void test_OptionalBool_constructor_false(void) {
    OptionalBool opt(false);
    TEST_ASSERT_EQUAL(OptionalBool::False, opt.state);
    TEST_ASSERT_TRUE(opt.isSet());
    TEST_ASSERT_FALSE(opt.getValue());
}

// Test isSet method
void test_OptionalBool_isSet_unset(void) {
    OptionalBool opt;
    TEST_ASSERT_FALSE(opt.isSet());
}

void test_OptionalBool_isSet_true(void) {
    OptionalBool opt(true);
    TEST_ASSERT_TRUE(opt.isSet());
}

void test_OptionalBool_isSet_false(void) {
    OptionalBool opt(false);
    TEST_ASSERT_TRUE(opt.isSet());
}

// Test getValue method
void test_OptionalBool_getValue_true(void) {
    OptionalBool opt(true);
    TEST_ASSERT_TRUE(opt.getValue());
}

void test_OptionalBool_getValue_false(void) {
    OptionalBool opt(false);
    TEST_ASSERT_FALSE(opt.getValue());
}

void test_OptionalBool_getValue_unset(void) {
    OptionalBool opt;
    // getValue() should return false when unset (state != True)
    TEST_ASSERT_FALSE(opt.getValue());
}

// Test equality operator
void test_OptionalBool_equality_both_unset(void) {
    OptionalBool opt1;
    OptionalBool opt2;
    TEST_ASSERT_TRUE(opt1 == opt2);
}

void test_OptionalBool_equality_both_true(void) {
    OptionalBool opt1(true);
    OptionalBool opt2(true);
    TEST_ASSERT_TRUE(opt1 == opt2);
}

void test_OptionalBool_equality_both_false(void) {
    OptionalBool opt1(false);
    OptionalBool opt2(false);
    TEST_ASSERT_TRUE(opt1 == opt2);
}

void test_OptionalBool_equality_different_states(void) {
    OptionalBool opt1(true);
    OptionalBool opt2(false);
    TEST_ASSERT_FALSE(opt1 == opt2);
}

void test_OptionalBool_equality_unset_vs_true(void) {
    OptionalBool opt1;
    OptionalBool opt2(true);
    TEST_ASSERT_FALSE(opt1 == opt2);
}

// Test inequality operator
void test_OptionalBool_inequality_same(void) {
    OptionalBool opt1(true);
    OptionalBool opt2(true);
    TEST_ASSERT_FALSE(opt1 != opt2);
}

void test_OptionalBool_inequality_different(void) {
    OptionalBool opt1(true);
    OptionalBool opt2(false);
    TEST_ASSERT_TRUE(opt1 != opt2);
}

// Test state transitions
void test_OptionalBool_state_assignment(void) {
    OptionalBool opt;
    TEST_ASSERT_EQUAL(OptionalBool::Unset, opt.state);
    
    opt.state = OptionalBool::True;
    TEST_ASSERT_EQUAL(OptionalBool::True, opt.state);
    TEST_ASSERT_TRUE(opt.isSet());
    TEST_ASSERT_TRUE(opt.getValue());
    
    opt.state = OptionalBool::False;
    TEST_ASSERT_EQUAL(OptionalBool::False, opt.state);
    TEST_ASSERT_TRUE(opt.isSet());
    TEST_ASSERT_FALSE(opt.getValue());
    
    opt.state = OptionalBool::Unset;
    TEST_ASSERT_EQUAL(OptionalBool::Unset, opt.state);
    TEST_ASSERT_FALSE(opt.isSet());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Constructor tests
    RUN_TEST(test_OptionalBool_constructor_default);
    RUN_TEST(test_OptionalBool_constructor_true);
    RUN_TEST(test_OptionalBool_constructor_false);
    
    // isSet tests
    RUN_TEST(test_OptionalBool_isSet_unset);
    RUN_TEST(test_OptionalBool_isSet_true);
    RUN_TEST(test_OptionalBool_isSet_false);
    
    // getValue tests
    RUN_TEST(test_OptionalBool_getValue_true);
    RUN_TEST(test_OptionalBool_getValue_false);
    RUN_TEST(test_OptionalBool_getValue_unset);
    
    // Equality tests
    RUN_TEST(test_OptionalBool_equality_both_unset);
    RUN_TEST(test_OptionalBool_equality_both_true);
    RUN_TEST(test_OptionalBool_equality_both_false);
    RUN_TEST(test_OptionalBool_equality_different_states);
    RUN_TEST(test_OptionalBool_equality_unset_vs_true);
    
    // Inequality tests
    RUN_TEST(test_OptionalBool_inequality_same);
    RUN_TEST(test_OptionalBool_inequality_different);
    
    // State transition tests
    RUN_TEST(test_OptionalBool_state_assignment);
    
    return UNITY_END();
}
