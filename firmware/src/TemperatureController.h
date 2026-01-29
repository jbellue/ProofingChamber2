#pragma once

#include <Arduino.h>
#include "services/IStorage.h"
#include "ITemperatureController.h"

class TemperatureController : public ITemperatureController {
public:
    TemperatureController(const gpio_num_t heaterPin, const gpio_num_t coolerPin, const gpio_num_t proofingLedPin, const gpio_num_t coolingLedPin);
    void setStorage(services::IStorage* storage);
    void setDefaultLimits(int8_t lower, int8_t higher);

    void begin() override;
    void setMode(Mode mode) override;
    void update(float currentTemp) override;
    Mode getMode() const override;

    bool isHeating() const override;
    bool isCooling() const override;

private:
    const gpio_num_t _heaterPin;
    const gpio_num_t _coolerPin;
    const gpio_num_t _proofingLedPin;
    const gpio_num_t _coolingLedPin;
    Mode _currentMode;
    services::IStorage* _storage;

    int8_t _lowerLimit;
    int8_t _higherLimit;

    bool _isHeating;
    bool _isCooling;

    void loadTemperatureSettings();
    void updateRelays(const float currentTemp);

    void turnHeater(bool on);
    void turnCooler(bool on);
};
