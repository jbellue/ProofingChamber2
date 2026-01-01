#include "mock_Arduino.h"

// Define the mock pin state storage
namespace arduino_mock {
    std::map<uint8_t, uint8_t> pinModes;
    std::map<uint8_t, uint8_t> pinStates;
}
