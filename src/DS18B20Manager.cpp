#include <OneWire.h>
#include <DallasTemperature.h>
#include "DS18B20Manager.h"
#include "DebugUtils.h"

DS18B20Manager::DS18B20Manager(const uint8_t oneWirePin):
        _oneWire(oneWirePin), _sensors(&_oneWire), _lastTemperature(0.0),
        _lastRequestTime(0), _currentResolution(9), _currentState(State::STOPPED) { }

void DS18B20Manager::begin() {
    _sensors.begin();
    if (_sensors.getAddress(_deviceAddress, 0)) {
        _currentState = State::STOPPED;  // Start in stopped state
        setResolution(9);
    } else {
        _currentState = State::ERROR;
        DEBUG_PRINTLN("DS18B20 not found!");
    }
}

void DS18B20Manager::startConversion() {
    _sensors.requestTemperatures();
    _lastRequestTime = millis();
}

void DS18B20Manager::update() {
    if (_currentState == State::STOPPED) return;
    handleState();
}

void DS18B20Manager::startPolling() {
    if (_currentState == State::STOPPED) {
        _currentState = State::WAITING_CONVERSION;
        startConversion();
    }
}

void DS18B20Manager::stopPolling() {
    _currentState = State::STOPPED;
}

void DS18B20Manager::handleState() {
    switch (_currentState) {
        case State::WAITING_CONVERSION:
            if (millis() - _lastRequestTime >= _conversionDelays[_currentResolution - 9]) {
                _currentState = State::READING_TEMP;
            }
            break;

        case State::READING_TEMP: {
            const float temp = _sensors.getTempCByIndex(0);
            if (temp != DEVICE_DISCONNECTED_C) {
                _lastTemperature = temp;

                if (_currentResolution == 9) {
                    setResolution(12);
                }

                startConversion();
                _currentState = State::WAITING_CONVERSION;
            } else {
                DEBUG_PRINTLN("Error reading temperature!");
                _currentState = State::ERROR;
            }
            break;
        }

        case State::ERROR:
            // Could implement retry logic here
            break;

        case State::STOPPED:
            // Do nothing while stopped
            break;
    }
}

// Set resolution (9-12 bits)
void DS18B20Manager::setResolution(uint8_t bits) {
    bits = constrain(bits, 9, 12);
    _sensors.setResolution(_deviceAddress, bits);
    _currentResolution = bits;
}

// Get the last temperature reading
float DS18B20Manager::getTemperature() const {
    return _lastTemperature;
}
