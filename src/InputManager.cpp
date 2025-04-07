#include "InputManager.h"
#include "DebugUtils.h"
#include <Arduino.h>

InputManager::InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, uint8_t ds18b20Pin) :
    _encoder(clkPin, dtPin, RotaryEncoder::LatchMode::FOUR3), _encoderSWPin(swPin), _buttonPressed(false),
    _lastButtonState(HIGH), _buttonState(HIGH), _lastDebounceTime(0), _ds18b20Manager(ds18b20Pin), _initialized(false) {}


void InputManager::resetEncoderPosition() {
    _encoder.setPosition(0);
    _lastEncoderPosition = 0;
    _lastDirection = EncoderDirection::None;
}

void InputManager::begin() {
    if (!_initialized) {
        pinMode(_encoderSWPin, INPUT_PULLUP);
        _ds18b20Manager.begin();
        _ds18b20Manager.setSlowPolling(true);
        _ds18b20Manager.startPolling();
        _initialized = true;
    }
    _encoder.tick();
    resetEncoderPosition();
}

void InputManager::update() {
    // Handle encoder rotation
    _encoder.tick();
    const int64_t newPosition = _encoder.getPosition();
    if (newPosition != _lastEncoderPosition) {
        _lastDirection = (newPosition > _lastEncoderPosition) ?
            EncoderDirection::Clockwise :
            EncoderDirection::CounterClockwise;
        _lastEncoderPosition = newPosition;
    }

    // Handle button press
    const int reading = digitalRead(_encoderSWPin);
    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }
    if ((millis() - _lastDebounceTime) > _debounceDelay) {
        if (reading != _buttonState) {
            _buttonState = reading;
            if (_buttonState == LOW) {
                _buttonPressed = true;
            }
        }
    }
    _lastButtonState = reading;

    _ds18b20Manager.update();
}

InputManager::EncoderDirection InputManager::getEncoderDirection() {
    const EncoderDirection direction = _lastDirection;
    _lastDirection = EncoderDirection::None;
    return direction;
}

bool InputManager::isButtonPressed() {
    if (_buttonPressed) {
        _buttonPressed = false; // Clear the flag
        return true;
    }
    return false;
}


void InputManager::slowTemperaturePolling(bool slowPolling) {
    _ds18b20Manager.setSlowPolling(slowPolling);
}

float InputManager::getTemperature() const {
    return _ds18b20Manager.getTemperature();
}
