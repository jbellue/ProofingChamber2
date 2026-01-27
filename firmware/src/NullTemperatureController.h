#pragma once

#include "ITemperatureController.h"
#include "DebugUtils.h"

// Null Object Pattern: Safe do-nothing implementation
class NullTemperatureController : public ITemperatureController {
public:
    static NullTemperatureController& getInstance() {
        static NullTemperatureController instance;
        return instance;
    }

    void begin() override {
        DEBUG_PRINTLN("NullTemperatureController: begin() called - no-op");
    }

    void setMode(ITemperatureController::Mode mode) override {
        DEBUG_PRINT("NullTemperatureController: setMode(");
        DEBUG_PRINT(mode);
        DEBUG_PRINTLN(") called - no-op");
    }

    void update(float currentTemp) override {
        // Silent no-op - called frequently
    }

    ITemperatureController::Mode getMode() const override {
        // Silent no-op - called frequently
        return ITemperatureController::OFF;
    }

    bool isHeating() const override {
        return false;
    }

    bool isCooling() const override {
        return false;
    }

private:
    NullTemperatureController() = default;
    NullTemperatureController(const NullTemperatureController&) = delete;
    NullTemperatureController& operator=(const NullTemperatureController&) = delete;
};
