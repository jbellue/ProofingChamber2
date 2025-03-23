#include "InputManager.h"
#include "DebugUtils.h"
#include <Arduino.h>

InputManager::InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin)
    : _encoder(clkPin, dtPin, RotaryEncoder::LatchMode::FOUR3), _encoderSWPin(swPin), _lastButtonState(HIGH), _buttonState(HIGH), _lastDebounceTime(0) {}

void InputManager::begin() {
    pinMode(_encoderSWPin, INPUT_PULLUP);
    update();
}

void InputManager::update() {
    _encoder.tick();
    //TODO maybe this should return a direction?
}

int64_t InputManager::getEncoderPosition() {
    return _encoder.getPosition();
}

bool InputManager::isButtonPressed() {
    const int reading = digitalRead(_encoderSWPin);
    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }
    if ((millis() - _lastDebounceTime) > 50) {
        if (reading != _buttonState) {
            _buttonState = reading;
            if (_buttonState == LOW) {
                return true;
            }
        }
    }
    _lastButtonState = reading;
    return false;
}
