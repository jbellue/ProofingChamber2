#include "InputManager.h"
#include "DebugUtils.h"
#include <esp_timer.h>
#include <driver/gpio.h>

InputManager::InputManager(const gpio_num_t clkPin, gpio_num_t dtPin, gpio_num_t swPin, gpio_num_t ds18b20Pin) :
        _encoder(clkPin, dtPin, RotaryEncoder::LatchMode::FOUR3), _encoderClk(clkPin),
        _encoderDt(dtPin), _encoderSWPin(swPin), _buttonPressed(false),
        _lastButtonState(1), _buttonState(1), _lastDebounceTime(0), _ds18b20Manager(ds18b20Pin),
        _initialized(false), _lastEncoderPosition(0), _pendingSteps(0), _buttonIrq(false), _lastRawButtonReading(1)
{
    _encoder.setPosition(0);
}


void InputManager::resetEncoderPosition() {
    _encoder.setPosition(0);
    _lastEncoderPosition = 0;
    _pendingSteps = 0;
}

void InputManager::begin() {
    if (!_initialized) {
        initialiseEncoderISR();
        _ds18b20Manager.begin();
        _ds18b20Manager.setSlowPolling(true);
        _ds18b20Manager.startPolling();
        _initialized = true;
    }
    resetEncoderPosition();
}

void InputManager::initialiseEncoderISR()
{
    // Configure encoder pins using ESP-IDF for fast ISR reads
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << _encoderClk) | (1ULL << _encoderDt) | (1ULL << _encoderSWPin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&cfg);
    gpio_install_isr_service(0);
    // Register handlers with context
    gpio_isr_handler_add(_encoderClk, InputManager::isrEncoder, this);
    gpio_isr_handler_add(_encoderDt, InputManager::isrEncoder, this);
    gpio_isr_handler_add(_encoderSWPin, InputManager::isrButton, this);
}

void InputManager::update() {
    const long pos = _encoder.getPosition();
    const long delta = pos - _lastEncoderPosition;
    if (delta != 0) {
        // accumulate steps; positive for CW, negative for CCW
        _pendingSteps += (int)delta;
        _lastEncoderPosition = pos;
    }

    // Handle button press with ISR edge gating and debounce
    if (_buttonIrq) {
        _buttonIrq = false;
        _lastDebounceTime = (unsigned long)(esp_timer_get_time() / 1000ULL);
    }
    if (((unsigned long)(esp_timer_get_time() / 1000ULL) - _lastDebounceTime) > _debounceDelay) {
        if (_lastRawButtonReading != _buttonState) {
            _buttonState = _lastRawButtonReading;
            if (_buttonState == 0) {
                _buttonPressed = true;
            }
        }
        _lastButtonState = _lastRawButtonReading;
    }

    _ds18b20Manager.update();
}

IInputManager::EncoderDirection InputManager::getEncoderDirection() {
    if (_pendingSteps > 0) {
        _pendingSteps--;
        return IInputManager::EncoderDirection::Clockwise;
    }
    if (_pendingSteps < 0) {
        _pendingSteps++;
        return IInputManager::EncoderDirection::CounterClockwise;
    }
    return IInputManager::EncoderDirection::None;
}

int InputManager::getPendingSteps() const {
    return _pendingSteps >= 0 ? _pendingSteps : -_pendingSteps;
}

void IRAM_ATTR InputManager::isrEncoder(void* arg) {
    auto* self = static_cast<InputManager*>(arg);
    const int s1 = gpio_get_level(self->_encoderClk);
    const int s2 = gpio_get_level(self->_encoderDt);
    self->_encoder.tick(s1, s2);
}

void IRAM_ATTR InputManager::isrButton(void* arg) {
    auto* self = static_cast<InputManager*>(arg);
    self->_lastRawButtonReading = gpio_get_level(self->_encoderSWPin);
    self->_buttonIrq = true;
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

// Virtual input injection for web interface
void InputManager::injectButtonPress() {
    // Simulate a button press by setting the flag
    _buttonPressed = true;
}

void InputManager::injectEncoderSteps(int steps) {
    // Add steps to pending steps counter
    // Positive for clockwise, negative for counter-clockwise
    _pendingSteps += steps;
}
