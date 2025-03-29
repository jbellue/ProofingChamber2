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

    InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, DS18B20Manager* ds18b20Manager);
    void begin();
    void update();
    bool isButtonPressed();
    EncoderDirection getEncoderDirection();
    void startTemperaturePolling();
    void stopTemperaturePolling();
    float getTemperature() const;

private:
    RotaryEncoder _encoder;
    DS18B20Manager* _ds18b20Manager;
    uint8_t _encoderSWPin;
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;
    int64_t _lastEncoderPosition;
    EncoderDirection _lastDirection;
};
