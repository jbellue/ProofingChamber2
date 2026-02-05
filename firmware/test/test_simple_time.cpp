// Unit tests for SimpleTime class
#include <gtest/gtest.h>
#include "SimpleTime.h"

class SimpleTimeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
    
    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test constructor
TEST_F(SimpleTimeTest, Constructor) {
    SimpleTime time1;
    EXPECT_EQ(time1.days, 0);
    EXPECT_EQ(time1.hours, 0);
    EXPECT_EQ(time1.minutes, 0);
    
    SimpleTime time2(1, 12, 30);
    EXPECT_EQ(time2.days, 1);
    EXPECT_EQ(time2.hours, 12);
    EXPECT_EQ(time2.minutes, 30);
}

// Test increment minutes
TEST_F(SimpleTimeTest, IncrementMinutes) {
    SimpleTime time(0, 0, 0);
    
    time.incrementMinutes();
    EXPECT_EQ(time.minutes, 1);
    EXPECT_EQ(time.hours, 0);
    EXPECT_EQ(time.days, 0);
}

TEST_F(SimpleTimeTest, IncrementMinutesOverflow) {
    SimpleTime time(0, 0, 59);
    
    time.incrementMinutes();
    EXPECT_EQ(time.minutes, 0);
    EXPECT_EQ(time.hours, 1);
    EXPECT_EQ(time.days, 0);
}

// Test increment hours
TEST_F(SimpleTimeTest, IncrementHours) {
    SimpleTime time(0, 0, 0);
    
    time.incrementHours();
    EXPECT_EQ(time.hours, 1);
    EXPECT_EQ(time.days, 0);
}

TEST_F(SimpleTimeTest, IncrementHoursOverflow) {
    SimpleTime time(0, 23, 0);
    
    time.incrementHours();
    EXPECT_EQ(time.hours, 0);
    EXPECT_EQ(time.days, 1);
}

// Test decrement minutes
TEST_F(SimpleTimeTest, DecrementMinutes) {
    SimpleTime time(0, 0, 30);
    
    time.decrementMinutes();
    EXPECT_EQ(time.minutes, 29);
    EXPECT_EQ(time.hours, 0);
}

TEST_F(SimpleTimeTest, DecrementMinutesUnderflow) {
    SimpleTime time(0, 1, 0);
    
    time.decrementMinutes();
    EXPECT_EQ(time.minutes, 59);
    EXPECT_EQ(time.hours, 0);
}

TEST_F(SimpleTimeTest, DecrementMinutesWithDays) {
    SimpleTime time(1, 0, 0);
    
    time.decrementMinutes();
    EXPECT_EQ(time.minutes, 59);
    EXPECT_EQ(time.hours, 23);
    EXPECT_EQ(time.days, 0);
}

// Test decrement hours
TEST_F(SimpleTimeTest, DecrementHours) {
    SimpleTime time(0, 5, 0);
    
    time.decrementHours();
    EXPECT_EQ(time.hours, 4);
    EXPECT_EQ(time.days, 0);
}

TEST_F(SimpleTimeTest, DecrementHoursUnderflow) {
    SimpleTime time(1, 0, 0);
    
    time.decrementHours();
    EXPECT_EQ(time.hours, 23);
    EXPECT_EQ(time.days, 0);
}

TEST_F(SimpleTimeTest, DecrementHoursAtZero) {
    SimpleTime time(0, 0, 0);
    
    time.decrementHours();
    EXPECT_EQ(time.hours, 0);
    EXPECT_EQ(time.days, 0);
}

// Test comparison operators
TEST_F(SimpleTimeTest, EqualityOperator) {
    SimpleTime time1(1, 12, 30);
    SimpleTime time2(1, 12, 30);
    SimpleTime time3(1, 12, 31);
    
    EXPECT_TRUE(time1 == time2);
    EXPECT_FALSE(time1 == time3);
}

TEST_F(SimpleTimeTest, InequalityOperator) {
    SimpleTime time1(1, 12, 30);
    SimpleTime time2(1, 12, 31);
    
    EXPECT_TRUE(time1 != time2);
    EXPECT_FALSE(time1 != time1);
}

TEST_F(SimpleTimeTest, GreaterThanOperator) {
    SimpleTime time1(1, 12, 30);
    SimpleTime time2(0, 12, 30);
    SimpleTime time3(1, 11, 30);
    SimpleTime time4(1, 12, 29);
    
    EXPECT_TRUE(time1 > time2);
    EXPECT_TRUE(time1 > time3);
    EXPECT_TRUE(time1 > time4);
    EXPECT_FALSE(time1 > time1);
}

TEST_F(SimpleTimeTest, LessThanOperator) {
    SimpleTime time1(0, 12, 30);
    SimpleTime time2(1, 12, 30);
    
    EXPECT_TRUE(time1 < time2);
    EXPECT_FALSE(time2 < time1);
    EXPECT_FALSE(time1 < time1);
}

TEST_F(SimpleTimeTest, GreaterThanOrEqualOperator) {
    SimpleTime time1(1, 12, 30);
    SimpleTime time2(1, 12, 30);
    SimpleTime time3(0, 12, 30);
    
    EXPECT_TRUE(time1 >= time2);
    EXPECT_TRUE(time1 >= time3);
    EXPECT_FALSE(time3 >= time1);
}

TEST_F(SimpleTimeTest, LessThanOrEqualOperator) {
    SimpleTime time1(1, 12, 30);
    SimpleTime time2(1, 12, 30);
    SimpleTime time3(2, 12, 30);
    
    EXPECT_TRUE(time1 <= time2);
    EXPECT_TRUE(time1 <= time3);
    EXPECT_FALSE(time3 <= time1);
}

// Test complex scenarios
TEST_F(SimpleTimeTest, MultipleIncrements) {
    SimpleTime time(0, 23, 58);
    
    time.incrementMinutes();  // Should be 0:23:59
    EXPECT_EQ(time.days, 0);
    EXPECT_EQ(time.hours, 23);
    EXPECT_EQ(time.minutes, 59);
    
    time.incrementMinutes();  // Should be 1:00:00
    EXPECT_EQ(time.days, 1);
    EXPECT_EQ(time.hours, 0);
    EXPECT_EQ(time.minutes, 0);
}

TEST_F(SimpleTimeTest, MultipleDecrements) {
    SimpleTime time(1, 0, 1);
    
    time.decrementMinutes();  // Should be 1:00:00
    EXPECT_EQ(time.days, 1);
    EXPECT_EQ(time.hours, 0);
    EXPECT_EQ(time.minutes, 0);
    
    time.decrementMinutes();  // Should be 0:23:59
    EXPECT_EQ(time.days, 0);
    EXPECT_EQ(time.hours, 23);
    EXPECT_EQ(time.minutes, 59);
}
