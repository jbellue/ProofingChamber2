#pragma once

#include <driver/gpio.h>
#include <RotaryEncoder.h>
#include "DS18B20Manager.h"
#include "IInputManager.h"

class InputManager : public IInputManager {
public:
    InputManager(const gpio_num_t clkPin, gpio_num_t dtPin, gpio_num_t swPin, gpio_num_t ds18b20Pin);
    void begin() override;
    void initialiseEncoderISR();
    void update() override;
    void resetEncoderPosition() override;
    bool isButtonPressed() override;
    IInputManager::EncoderDirection getEncoderDirection() override;
    int getPendingSteps() const override;
    void slowTemperaturePolling(bool slowPolling) override;
    float getTemperature() const override;
    
    // Virtual input injection for web interface
    void injectButtonPress() override;
    void injectEncoderSteps(int steps) override;

private:
    static void isrEncoder(void* arg);

    RotaryEncoder _encoder;
    DS18B20Manager _ds18b20Manager;
    gpio_num_t _encoderSWPin;
    // Fast GPIO identifiers for ISR-level reads
    gpio_num_t _encoderClk;
    gpio_num_t _encoderDt;
    volatile bool _buttonIrq;
    int _lastButtonState;
    int _lastRawButtonReading;
    int _buttonState;
    bool _initialized;
    unsigned long _lastDebounceTime;
    long _lastEncoderPosition;
    int _pendingSteps;
    bool _buttonPressed;
    const unsigned long _debounceDelay = 50;
    static void isrButton(void* arg);
};
