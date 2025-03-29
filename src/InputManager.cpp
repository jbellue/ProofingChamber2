#include "InputManager.h"
#include "DebugUtils.h"
#include <Arduino.h>

InputManager::InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, DS18B20Manager* ds18b20Manager) :
    _encoder(clkPin, dtPin, RotaryEncoder::LatchMode::FOUR3), _encoderSWPin(swPin),
    _lastButtonState(HIGH), _buttonState(HIGH), _lastDebounceTime(0), _ds18b20Manager(ds18b20Manager) {}

void InputManager::begin() {
    pinMode(_encoderSWPin, INPUT_PULLUP);
    _ds18b20Manager->begin();
    _encoder.tick();
    _lastEncoderPosition = _encoder.getPosition();
    _lastDirection = EncoderDirection::None;
}

void InputManager::update() {
    _ds18b20Manager->update();
    _encoder.tick();
    const int64_t newPosition = _encoder.getPosition();
    if (newPosition != _lastEncoderPosition) {
        _lastDirection = (newPosition > _lastEncoderPosition) ?
            EncoderDirection::Clockwise :
            EncoderDirection::CounterClockwise;
        _lastEncoderPosition = newPosition;
    }
}

InputManager::EncoderDirection InputManager::getEncoderDirection() {
    const EncoderDirection direction = _lastDirection;
    _lastDirection = EncoderDirection::None;
    return direction;
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


void InputManager::startTemperaturePolling() {
    _ds18b20Manager->startPolling();
}

void InputManager::stopTemperaturePolling() {
    _ds18b20Manager->stopPolling();
}

float InputManager::getTemperature() const {
    return _ds18b20Manager->getTemperature();
}
