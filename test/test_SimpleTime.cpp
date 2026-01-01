#include <unity.h>
#include "../src/SimpleTime.h"

void setUp(void) {
    // set up code here, to run before every test
}

void tearDown(void) {
    // clean up code here, to run after every test
}

// Test constructor and initialization
void test_SimpleTime_constructor_default(void) {
    SimpleTime t;
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_constructor_with_values(void) {
    SimpleTime t(2, 15, 30);
    TEST_ASSERT_EQUAL(2, t.days);
    TEST_ASSERT_EQUAL(15, t.hours);
    TEST_ASSERT_EQUAL(30, t.minutes);
}

// Test incrementHours
void test_SimpleTime_incrementHours_normal(void) {
    SimpleTime t(0, 10, 0);
    t.incrementHours();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(11, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_incrementHours_rollover(void) {
    SimpleTime t(0, 23, 0);
    t.incrementHours();
    TEST_ASSERT_EQUAL(1, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_incrementHours_multiple_days(void) {
    SimpleTime t(5, 23, 0);
    t.incrementHours();
    TEST_ASSERT_EQUAL(6, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

// Test incrementMinutes
void test_SimpleTime_incrementMinutes_normal(void) {
    SimpleTime t(0, 0, 30);
    t.incrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(31, t.minutes);
}

void test_SimpleTime_incrementMinutes_rollover_to_hour(void) {
    SimpleTime t(0, 10, 59);
    t.incrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(11, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_incrementMinutes_rollover_to_day(void) {
    SimpleTime t(0, 23, 59);
    t.incrementMinutes();
    TEST_ASSERT_EQUAL(1, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

// Test decrementHours
void test_SimpleTime_decrementHours_normal(void) {
    SimpleTime t(0, 10, 0);
    t.decrementHours();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(9, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_decrementHours_from_zero(void) {
    SimpleTime t(0, 0, 0);
    t.decrementHours();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_decrementHours_rollover_from_day(void) {
    SimpleTime t(1, 0, 0);
    t.decrementHours();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(23, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_decrementHours_multiple_days(void) {
    SimpleTime t(5, 0, 0);
    t.decrementHours();
    TEST_ASSERT_EQUAL(4, t.days);
    TEST_ASSERT_EQUAL(23, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

// Test decrementMinutes
void test_SimpleTime_decrementMinutes_normal(void) {
    SimpleTime t(0, 0, 30);
    t.decrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(29, t.minutes);
}

void test_SimpleTime_decrementMinutes_from_zero(void) {
    SimpleTime t(0, 0, 0);
    t.decrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(0, t.minutes);
}

void test_SimpleTime_decrementMinutes_rollover_from_hour(void) {
    SimpleTime t(0, 1, 0);
    t.decrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(0, t.hours);
    TEST_ASSERT_EQUAL(59, t.minutes);
}

void test_SimpleTime_decrementMinutes_rollover_from_day(void) {
    SimpleTime t(1, 0, 0);
    t.decrementMinutes();
    TEST_ASSERT_EQUAL(0, t.days);
    TEST_ASSERT_EQUAL(23, t.hours);
    TEST_ASSERT_EQUAL(59, t.minutes);
}

void test_SimpleTime_decrementMinutes_with_days_and_hours(void) {
    SimpleTime t(2, 5, 0);
    t.decrementMinutes();
    TEST_ASSERT_EQUAL(2, t.days);
    TEST_ASSERT_EQUAL(4, t.hours);
    TEST_ASSERT_EQUAL(59, t.minutes);
}

// Test comparison operators
void test_SimpleTime_equality_equal(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 30);
    TEST_ASSERT_TRUE(t1 == t2);
}

void test_SimpleTime_equality_not_equal(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 31);
    TEST_ASSERT_FALSE(t1 == t2);
}

void test_SimpleTime_inequality(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 31);
    TEST_ASSERT_TRUE(t1 != t2);
}

void test_SimpleTime_greater_than_by_days(void) {
    SimpleTime t1(2, 10, 30);
    SimpleTime t2(1, 10, 30);
    TEST_ASSERT_TRUE(t1 > t2);
    TEST_ASSERT_FALSE(t2 > t1);
}

void test_SimpleTime_greater_than_by_hours(void) {
    SimpleTime t1(1, 11, 30);
    SimpleTime t2(1, 10, 30);
    TEST_ASSERT_TRUE(t1 > t2);
    TEST_ASSERT_FALSE(t2 > t1);
}

void test_SimpleTime_greater_than_by_minutes(void) {
    SimpleTime t1(1, 10, 31);
    SimpleTime t2(1, 10, 30);
    TEST_ASSERT_TRUE(t1 > t2);
    TEST_ASSERT_FALSE(t2 > t1);
}

void test_SimpleTime_less_than(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 31);
    TEST_ASSERT_TRUE(t1 < t2);
    TEST_ASSERT_FALSE(t2 < t1);
}

void test_SimpleTime_greater_than_or_equal(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 30);
    SimpleTime t3(1, 10, 29);
    TEST_ASSERT_TRUE(t1 >= t2);
    TEST_ASSERT_TRUE(t1 >= t3);
    TEST_ASSERT_FALSE(t3 >= t1);
}

void test_SimpleTime_less_than_or_equal(void) {
    SimpleTime t1(1, 10, 30);
    SimpleTime t2(1, 10, 30);
    SimpleTime t3(1, 10, 31);
    TEST_ASSERT_TRUE(t1 <= t2);
    TEST_ASSERT_TRUE(t1 <= t3);
    TEST_ASSERT_FALSE(t3 <= t1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Constructor tests
    RUN_TEST(test_SimpleTime_constructor_default);
    RUN_TEST(test_SimpleTime_constructor_with_values);
    
    // incrementHours tests
    RUN_TEST(test_SimpleTime_incrementHours_normal);
    RUN_TEST(test_SimpleTime_incrementHours_rollover);
    RUN_TEST(test_SimpleTime_incrementHours_multiple_days);
    
    // incrementMinutes tests
    RUN_TEST(test_SimpleTime_incrementMinutes_normal);
    RUN_TEST(test_SimpleTime_incrementMinutes_rollover_to_hour);
    RUN_TEST(test_SimpleTime_incrementMinutes_rollover_to_day);
    
    // decrementHours tests
    RUN_TEST(test_SimpleTime_decrementHours_normal);
    RUN_TEST(test_SimpleTime_decrementHours_from_zero);
    RUN_TEST(test_SimpleTime_decrementHours_rollover_from_day);
    RUN_TEST(test_SimpleTime_decrementHours_multiple_days);
    
    // decrementMinutes tests
    RUN_TEST(test_SimpleTime_decrementMinutes_normal);
    RUN_TEST(test_SimpleTime_decrementMinutes_from_zero);
    RUN_TEST(test_SimpleTime_decrementMinutes_rollover_from_hour);
    RUN_TEST(test_SimpleTime_decrementMinutes_rollover_from_day);
    RUN_TEST(test_SimpleTime_decrementMinutes_with_days_and_hours);
    
    // Comparison operator tests
    RUN_TEST(test_SimpleTime_equality_equal);
    RUN_TEST(test_SimpleTime_equality_not_equal);
    RUN_TEST(test_SimpleTime_inequality);
    RUN_TEST(test_SimpleTime_greater_than_by_days);
    RUN_TEST(test_SimpleTime_greater_than_by_hours);
    RUN_TEST(test_SimpleTime_greater_than_by_minutes);
    RUN_TEST(test_SimpleTime_less_than);
    RUN_TEST(test_SimpleTime_greater_than_or_equal);
    RUN_TEST(test_SimpleTime_less_than_or_equal);
    
    return UNITY_END();
}
