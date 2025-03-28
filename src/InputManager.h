#pragma once

#include <RotaryEncoder.h>

class InputManager {
public:
    enum class EncoderDirection {
        None,
        Clockwise,
        CounterClockwise
    };

    InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin);
    void begin();
    void update();
    bool isButtonPressed();
    EncoderDirection getEncoderDirection();

private:
    RotaryEncoder _encoder;
    uint8_t _encoderSWPin;
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;
    int64_t _lastEncoderPosition;
    EncoderDirection _lastDirection;
};
