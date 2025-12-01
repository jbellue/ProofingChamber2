#pragma once

#include <Arduino.h>
#include "services/IStorage.h"
#include "ITemperatureController.h"

class TemperatureController : public ITemperatureController {
public:
    TemperatureController(const uint8_t heaterPin, const uint8_t coolerPin, const uint8_t proofingLedPin, const uint8_t coolingLedPin);
    void setStorage(services::IStorage* storage);

    void begin() override;
    void setMode(Mode mode) override;
    void update(float currentTemp) override;
    Mode getMode() const override;

    bool isHeating() const override;
    bool isCooling() const override;

private:
    const uint8_t _heaterPin;
    const uint8_t _coolerPin;
    const uint8_t _proofingLedPin;
    const uint8_t _coolingLedPin;
    Mode _currentMode;

    int8_t _targetTemp;
    int8_t _lowerLimit;
    int8_t _higherLimit;

    bool _isHeating;
    bool _isCooling;

    void loadTemperatureSettings();
    void updateRelays(const float currentTemp);

    void turnHeater(bool on);
    void turnCooler(bool on);
};
