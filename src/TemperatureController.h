#pragma once

#include <Arduino.h>
#include "services/IStorage.h"

class TemperatureController {
public:
    enum Mode {
        HEATING,
        COOLING,
        OFF
    };

    TemperatureController(const uint8_t heaterPin, const uint8_t coolerPin);
    void setStorage(services::IStorage* storage);

    void begin();
    void setMode(Mode mode);
    void update(float currentTemp);
    Mode getMode() const;

private:
    const uint8_t _heaterPin;
    const uint8_t _coolerPin;
    Mode _currentMode;

    int8_t _targetTemp;
    int8_t _lowerLimit;
    int8_t _higherLimit;

    void loadTemperatureSettings();
    void updateRelays(const float currentTemp);
};
