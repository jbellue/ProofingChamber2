// Mock DallasTemperature.h for unit testing
#pragma once

#include "OneWire.h"

class DallasTemperature {
public:
    DallasTemperature(OneWire* wire) {}
    void begin() {}
    void requestTemperatures() {}
    float getTempCByIndex(uint8_t index) { return 25.0f; } // Mock temperature
};
