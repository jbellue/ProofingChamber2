#pragma once

#include <Arduino.h>

class IInputManager {
public:
    enum class EncoderDirection {
        None,
        Clockwise,
        CounterClockwise
    };

    virtual ~IInputManager() = default;
    
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void resetEncoderPosition() = 0;
    virtual bool isButtonPressed() = 0;
    virtual EncoderDirection getEncoderDirection() = 0;
    virtual int getPendingSteps() const = 0;
    virtual void slowTemperaturePolling(bool slowPolling) = 0;
    virtual float getTemperature() const = 0;
    
    // Virtual input injection for web interface
    virtual void injectButtonPress() = 0;
    virtual void injectEncoderSteps(int steps) = 0;
};
