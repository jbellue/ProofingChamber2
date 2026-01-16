#pragma once

#include <RotaryEncoder.h>
#include "DS18B20Manager.h"
#include "IInputManager.h"

class InputManager : public IInputManager {
public:
    InputManager(uint8_t clkPin, uint8_t dtPin, uint8_t swPin, uint8_t ds18b20Pin);
    void begin() override;
    void update() override;
    void resetEncoderPosition() override;
    bool isButtonPressed() override;
    IInputManager::EncoderDirection getEncoderDirection() override;
    void slowTemperaturePolling(bool slowPolling) override;
    float getTemperature() const override;

private:
    RotaryEncoder _encoder;
    DS18B20Manager _ds18b20Manager;
    uint8_t _encoderSWPin;
    int _lastButtonState;
    int _buttonState;
    bool _initialized;
    unsigned long _lastDebounceTime;
    IInputManager::EncoderDirection _lastDirection;
    bool _buttonPressed;
    const unsigned long _debounceDelay = 50;
};
