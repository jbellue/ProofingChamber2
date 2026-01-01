#include <unity.h>
#include <string.h>
#include <map>
#include <string>

// Include mocks (they're in test directory which is in include path)
#include "mock_Arduino.h"

// Must define DEBUG before including any real headers
#ifndef DEBUG
#define DEBUG 0
#endif

#include "DebugUtils.h"

// Now include TemperatureController (it will find real IStorage.h)
#include "../src/ITemperatureController.h"
#include "../src/TemperatureController.h"

// Mock storage implementation
class MockStorage : public services::IStorage {
private:
    std::map<std::string, int> intStorage;
    
public:
    virtual ~MockStorage() {}
    
    bool begin() override { return true; }
    
    int readInt(const char* path, int defaultValue = 0) override {
        std::string key(path);
        if (intStorage.find(key) != intStorage.end()) {
            return intStorage[key];
        }
        return defaultValue;
    }
    
    float readFloat(const char* path, float defaultValue = 0.0f) override {
        return (float)readInt(path, (int)defaultValue);
    }
    
    bool readString(const char* path, char* buffer, size_t bufferSize, const char* defaultValue = "") override {
        strncpy(buffer, defaultValue, bufferSize);
        return true;
    }
    
    bool writeInt(const char* path, int value) override {
        intStorage[std::string(path)] = value;
        return true;
    }
    
    bool writeFloat(const char* path, float value) override {
        return writeInt(path, (int)value);
    }
    
    bool writeString(const char* path, const char* value) override {
        return true;
    }
    
    void setInt(const char* path, int value) {
        intStorage[std::string(path)] = value;
    }
};

void setUp(void) {
    // Clear mock states before each test
    arduino_mock::pinModes.clear();
    arduino_mock::pinStates.clear();
}

void tearDown(void) {
    // Clean up after each test
    arduino_mock::pinModes.clear();
    arduino_mock::pinStates.clear();
}

// Test constructor and initial state
void test_TemperatureController_constructor(void) {
    TemperatureController controller(2, 1, 6, 5);
    
    TEST_ASSERT_EQUAL(ITemperatureController::OFF, controller.getMode());
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test begin method sets up pins
void test_TemperatureController_begin_sets_pins(void) {
    TemperatureController controller(2, 1, 6, 5);
    
    controller.begin();
    
    // Verify pins were set to OUTPUT mode
    TEST_ASSERT_EQUAL(OUTPUT, arduino_mock::pinModes[2]);  // heater
    TEST_ASSERT_EQUAL(OUTPUT, arduino_mock::pinModes[1]);  // cooler
    TEST_ASSERT_EQUAL(OUTPUT, arduino_mock::pinModes[6]);  // proofing LED
    TEST_ASSERT_EQUAL(OUTPUT, arduino_mock::pinModes[5]);  // cooling LED
    
    // Verify relays start OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);  // heater OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);  // cooler OFF
}

// Test setMode to HEATING
void test_TemperatureController_setMode_heating(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(28, 35);
    
    controller.setMode(ITemperatureController::HEATING);
    
    TEST_ASSERT_EQUAL(ITemperatureController::HEATING, controller.getMode());
    TEST_ASSERT_EQUAL(HIGH, arduino_mock::pinStates[6]);  // proofing LED ON
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[5]);   // cooling LED OFF
}

// Test setMode to COOLING
void test_TemperatureController_setMode_cooling(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(2, 7);
    
    controller.setMode(ITemperatureController::COOLING);
    
    TEST_ASSERT_EQUAL(ITemperatureController::COOLING, controller.getMode());
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[6]);   // proofing LED OFF
    TEST_ASSERT_EQUAL(HIGH, arduino_mock::pinStates[5]);  // cooling LED ON
}

// Test setMode to OFF
void test_TemperatureController_setMode_off(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    
    // Set to heating first
    controller.setMode(ITemperatureController::HEATING);
    // Then turn off
    controller.setMode(ITemperatureController::OFF);
    
    TEST_ASSERT_EQUAL(ITemperatureController::OFF, controller.getMode());
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[6]);   // proofing LED OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[5]);   // cooling LED OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);   // heater OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);   // cooler OFF
}

// Test heating mode - temperature below lower limit turns heater ON
void test_TemperatureController_heating_below_lower_limit(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(28, 35);
    controller.setMode(ITemperatureController::HEATING);
    
    controller.update(25.0f);  // Below lower limit of 28
    
    TEST_ASSERT_EQUAL(HIGH, arduino_mock::pinStates[2]);  // heater ON
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);   // cooler OFF
    TEST_ASSERT_TRUE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test heating mode - temperature above upper limit turns heater OFF
void test_TemperatureController_heating_above_upper_limit(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(28, 35);
    controller.setMode(ITemperatureController::HEATING);
    
    // First turn heater on
    controller.update(25.0f);
    TEST_ASSERT_TRUE(controller.isHeating());
    
    // Then temperature rises above upper limit
    controller.update(36.0f);  // Above upper limit of 35
    
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);   // heater OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);   // cooler OFF
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test heating mode - temperature in hysteresis band maintains state
void test_TemperatureController_heating_in_hysteresis(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(28, 35);
    controller.setMode(ITemperatureController::HEATING);
    
    // Start below lower limit - heater turns ON
    controller.update(25.0f);
    TEST_ASSERT_TRUE(controller.isHeating());
    
    // Temperature rises into hysteresis band - heater should stay ON
    controller.update(30.0f);
    TEST_ASSERT_TRUE(controller.isHeating());
    
    controller.update(34.0f);
    TEST_ASSERT_TRUE(controller.isHeating());
}

// Test cooling mode - temperature above upper limit turns cooler ON
void test_TemperatureController_cooling_above_upper_limit(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(2, 7);
    controller.setMode(ITemperatureController::COOLING);
    
    controller.update(10.0f);  // Above upper limit of 7
    
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);   // heater OFF
    TEST_ASSERT_EQUAL(HIGH, arduino_mock::pinStates[1]);  // cooler ON
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_TRUE(controller.isCooling());
}

// Test cooling mode - temperature below lower limit turns cooler OFF
void test_TemperatureController_cooling_below_lower_limit(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(2, 7);
    controller.setMode(ITemperatureController::COOLING);
    
    // First turn cooler on
    controller.update(10.0f);
    TEST_ASSERT_TRUE(controller.isCooling());
    
    // Then temperature drops below lower limit
    controller.update(1.0f);  // Below lower limit of 2
    
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);   // heater OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);   // cooler OFF
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test cooling mode - temperature in hysteresis band maintains state
void test_TemperatureController_cooling_in_hysteresis(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(2, 7);
    controller.setMode(ITemperatureController::COOLING);
    
    // Start above upper limit - cooler turns ON
    controller.update(10.0f);
    TEST_ASSERT_TRUE(controller.isCooling());
    
    // Temperature drops into hysteresis band - cooler should stay ON
    controller.update(5.0f);
    TEST_ASSERT_TRUE(controller.isCooling());
    
    controller.update(3.0f);
    TEST_ASSERT_TRUE(controller.isCooling());
}

// Test OFF mode doesn't control relays
void test_TemperatureController_off_mode_no_control(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setMode(ITemperatureController::OFF);
    
    // Update with various temperatures - nothing should happen
    controller.update(0.0f);
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
    
    controller.update(50.0f);
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test mode switching turns off relays
void test_TemperatureController_mode_switch_safety(void) {
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setDefaultLimits(28, 35);
    
    // Start in heating mode with heater ON
    controller.setMode(ITemperatureController::HEATING);
    controller.update(25.0f);
    TEST_ASSERT_TRUE(controller.isHeating());
    
    // Switch to cooling mode - heater should turn OFF
    controller.setDefaultLimits(2, 7);
    controller.setMode(ITemperatureController::COOLING);
    
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[2]);   // heater OFF
    TEST_ASSERT_EQUAL(LOW, arduino_mock::pinStates[1]);   // cooler OFF
    TEST_ASSERT_FALSE(controller.isHeating());
    TEST_ASSERT_FALSE(controller.isCooling());
}

// Test with storage
void test_TemperatureController_with_storage(void) {
    MockStorage storage;
    storage.setInt("/hot/lower_limit.txt", 30);
    storage.setInt("/hot/higher_limit.txt", 38);
    
    TemperatureController controller(2, 1, 6, 5);
    controller.begin();
    controller.setStorage(&storage);
    controller.setMode(ITemperatureController::HEATING);
    
    // Should use storage values (30-38) not defaults
    controller.update(28.0f);  // Below 30
    TEST_ASSERT_TRUE(controller.isHeating());
    
    controller.update(39.0f);  // Above 38
    TEST_ASSERT_FALSE(controller.isHeating());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // Constructor tests
    RUN_TEST(test_TemperatureController_constructor);
    
    // Initialization tests
    RUN_TEST(test_TemperatureController_begin_sets_pins);
    
    // Mode setting tests
    RUN_TEST(test_TemperatureController_setMode_heating);
    RUN_TEST(test_TemperatureController_setMode_cooling);
    RUN_TEST(test_TemperatureController_setMode_off);
    
    // Heating mode tests
    RUN_TEST(test_TemperatureController_heating_below_lower_limit);
    RUN_TEST(test_TemperatureController_heating_above_upper_limit);
    RUN_TEST(test_TemperatureController_heating_in_hysteresis);
    
    // Cooling mode tests
    RUN_TEST(test_TemperatureController_cooling_above_upper_limit);
    RUN_TEST(test_TemperatureController_cooling_below_lower_limit);
    RUN_TEST(test_TemperatureController_cooling_in_hysteresis);
    
    // OFF mode tests
    RUN_TEST(test_TemperatureController_off_mode_no_control);
    
    // Safety tests
    RUN_TEST(test_TemperatureController_mode_switch_safety);
    
    // Storage tests
    RUN_TEST(test_TemperatureController_with_storage);
    
    return UNITY_END();
}
