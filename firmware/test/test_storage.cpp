// Unit tests for Storage class
#include <gtest/gtest.h>
#include <cstring>
#include "Storage.h"
#include "StorageConstants.h"

class StorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize storage before each test
        Storage::begin();
    }
    
    void TearDown() override {
        // Storage is static, so we can't easily reset it between tests
        // In a real implementation, we'd add a reset() method for testing
    }
};

// Test initialization
TEST_F(StorageTest, Initialization) {
    // begin() is called in SetUp()
    // Test that default values are set
    int hotLower = Storage::getInt(storage::keys::HOT_LOWER_LIMIT_KEY);
    int hotUpper = Storage::getInt(storage::keys::HOT_UPPER_LIMIT_KEY);
    int coldLower = Storage::getInt(storage::keys::COLD_LOWER_LIMIT_KEY);
    int coldUpper = Storage::getInt(storage::keys::COLD_UPPER_LIMIT_KEY);
    
    EXPECT_EQ(hotLower, storage::defaults::HOT_LOWER_LIMIT_DEFAULT);
    EXPECT_EQ(hotUpper, storage::defaults::HOT_UPPER_LIMIT_DEFAULT);
    EXPECT_EQ(coldLower, storage::defaults::COLD_LOWER_LIMIT_DEFAULT);
    EXPECT_EQ(coldUpper, storage::defaults::COLD_UPPER_LIMIT_DEFAULT);
}

// Test integer storage
TEST_F(StorageTest, IntegerReadWrite) {
    const char* testKey = "test_int";
    int testValue = 42;
    
    // Write value
    bool writeSuccess = Storage::setInt(testKey, testValue);
    EXPECT_TRUE(writeSuccess);
    
    // Read value back
    int readValue = Storage::getInt(testKey);
    EXPECT_EQ(readValue, testValue);
}

TEST_F(StorageTest, IntegerDefaultValue) {
    const char* nonExistentKey = "non_existent_key";
    int defaultValue = 99;
    
    int readValue = Storage::getInt(nonExistentKey, defaultValue);
    EXPECT_EQ(readValue, defaultValue);
}

// Test float storage
TEST_F(StorageTest, FloatReadWrite) {
    const char* testKey = "test_float";
    float testValue = 3.14159f;
    
    // Write value
    bool writeSuccess = Storage::setFloat(testKey, testValue);
    EXPECT_TRUE(writeSuccess);
    
    // Read value back
    float readValue = Storage::getFloat(testKey);
    EXPECT_FLOAT_EQ(readValue, testValue);
}

TEST_F(StorageTest, FloatDefaultValue) {
    const char* nonExistentKey = "non_existent_float";
    float defaultValue = 2.71828f;
    
    float readValue = Storage::getFloat(nonExistentKey, defaultValue);
    EXPECT_FLOAT_EQ(readValue, defaultValue);
}

// Test string storage
TEST_F(StorageTest, StringReadWrite) {
    const char* testKey = "test_string";
    const char* testValue = "Hello, World!";
    char buffer[100];
    
    // Write value
    bool writeSuccess = Storage::setCharArray(testKey, testValue);
    EXPECT_TRUE(writeSuccess);
    
    // Read value back
    bool readSuccess = Storage::getCharArray(testKey, buffer, sizeof(buffer));
    EXPECT_TRUE(readSuccess);
    EXPECT_STREQ(buffer, testValue);
}

TEST_F(StorageTest, StringDefaultValue) {
    const char* nonExistentKey = "non_existent_string";
    const char* defaultValue = "default";
    char buffer[100];
    
    bool readSuccess = Storage::getCharArray(nonExistentKey, buffer, sizeof(buffer), defaultValue);
    EXPECT_FALSE(readSuccess);  // Key doesn't exist
    EXPECT_STREQ(buffer, defaultValue);
}

// Test overwriting values
TEST_F(StorageTest, OverwriteInteger) {
    const char* testKey = "test_overwrite_int";
    
    Storage::setInt(testKey, 10);
    EXPECT_EQ(Storage::getInt(testKey), 10);
    
    Storage::setInt(testKey, 20);
    EXPECT_EQ(Storage::getInt(testKey), 20);
}

TEST_F(StorageTest, OverwriteString) {
    const char* testKey = "test_overwrite_string";
    char buffer[100];
    
    Storage::setCharArray(testKey, "first");
    Storage::getCharArray(testKey, buffer, sizeof(buffer));
    EXPECT_STREQ(buffer, "first");
    
    Storage::setCharArray(testKey, "second");
    Storage::getCharArray(testKey, buffer, sizeof(buffer));
    EXPECT_STREQ(buffer, "second");
}

// Test temperature limit keys
TEST_F(StorageTest, TemperatureLimits) {
    // Set hot limits
    Storage::setInt(storage::keys::HOT_LOWER_LIMIT_KEY, 28);
    Storage::setInt(storage::keys::HOT_UPPER_LIMIT_KEY, 35);
    
    EXPECT_EQ(Storage::getInt(storage::keys::HOT_LOWER_LIMIT_KEY), 28);
    EXPECT_EQ(Storage::getInt(storage::keys::HOT_UPPER_LIMIT_KEY), 35);
    
    // Set cold limits
    Storage::setInt(storage::keys::COLD_LOWER_LIMIT_KEY, 2);
    Storage::setInt(storage::keys::COLD_UPPER_LIMIT_KEY, 7);
    
    EXPECT_EQ(Storage::getInt(storage::keys::COLD_LOWER_LIMIT_KEY), 2);
    EXPECT_EQ(Storage::getInt(storage::keys::COLD_UPPER_LIMIT_KEY), 7);
}

// Test timezone key
TEST_F(StorageTest, TimezoneKey) {
    char buffer[100];
    const char* timezone = "Europe/Paris";
    
    Storage::setCharArray(storage::keys::TIMEZONE_KEY, timezone);
    Storage::getCharArray(storage::keys::TIMEZONE_KEY, buffer, sizeof(buffer));
    
    EXPECT_STREQ(buffer, timezone);
}
