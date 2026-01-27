#pragma once

#include <Arduino.h>

// Abstract interface for TemperatureController
class ITemperatureController {
public:
    enum Mode {
        HEATING,
        COOLING,
        OFF
    };
    virtual ~ITemperatureController() = default;

    virtual void begin() = 0;
    virtual void setMode(Mode mode) = 0;
    virtual void update(float currentTemp) = 0;
    virtual Mode getMode() const = 0;
    virtual bool isHeating() const = 0;
    virtual bool isCooling() const = 0;
};
