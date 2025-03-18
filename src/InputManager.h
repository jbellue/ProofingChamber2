#pragma once

#include <RotaryEncoder.h>

class InputManager {
public:
    InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin);
    void begin();
    void update();
    int64_t getEncoderPosition();
    bool isButtonPressed();

private:
    RotaryEncoder _encoder;
    uint8_t _encoderSWPin;
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;
};
