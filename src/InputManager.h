#pragma once

#include <RotaryEncoder.h>
#include "DS18B20Manager.h"

class InputManager {
public:
    enum class EncoderDirection {
        None,
        Clockwise,
        CounterClockwise
    };

    InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, uint8_t ds18b20Pin);
    void begin();
    void update();
    void resetEncoderPosition();
    bool isButtonPressed();
    EncoderDirection getEncoderDirection();
    void slowTemperaturePolling(bool slowPolling);
    float getTemperature() const;

private:
    RotaryEncoder _encoder;
    DS18B20Manager _ds18b20Manager;
    uint8_t _encoderSWPin;
    int _lastButtonState;
    int _buttonState;
    bool _initialized;
    unsigned long _lastDebounceTime;
    int64_t _lastEncoderPosition;
    EncoderDirection _lastDirection;
    bool _buttonPressed;
    const unsigned long _debounceDelay = 50;
};
