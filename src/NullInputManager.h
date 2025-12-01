#pragma once

#include "IInputManager.h"
#include "DebugUtils.h"

// Null Object Pattern: Safe do-nothing implementation
class NullInputManager : public IInputManager {
public:
    static NullInputManager& getInstance() {
        static NullInputManager instance;
        return instance;
    }

    void begin() override {
        DEBUG_PRINTLN("NullInputManager: begin() called - no-op");
    }

    void update() override {
        // Silent no-op - called frequently, don't spam logs
    }

    void resetEncoderPosition() override {
        DEBUG_PRINTLN("NullInputManager: resetEncoderPosition() called - no-op");
    }

    bool isButtonPressed() override {
        // Silent no-op - called frequently
        return false;
    }

    EncoderDirection getEncoderDirection() override {
        // Silent no-op - called frequently
        return EncoderDirection::None;
    }

    void slowTemperaturePolling(bool slowPolling) override {
        DEBUG_PRINTLN("NullInputManager: slowTemperaturePolling() called - no-op");
    }

    float getTemperature() const override {
        DEBUG_PRINTLN("NullInputManager: getTemperature() called - returning safe default");
        return 20.0f; // Safe room temperature default
    }

private:
    NullInputManager() = default;
    NullInputManager(const NullInputManager&) = delete;
    NullInputManager& operator=(const NullInputManager&) = delete;
};
