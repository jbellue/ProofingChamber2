#include <OneWire.h>
#include <DallasTemperature.h>
#include "DS18B20Manager.h"
#include "DebugUtils.h"

DS18B20Manager::DS18B20Manager(const uint8_t oneWirePin):
        _oneWire(oneWirePin), _sensors(&_oneWire), _lastTemperature(0.0),
        _currentResolution(9), _currentState(State::STOPPED),
        _lastErrorTime(0), _errorRetryCount(0), _slowPolling(true) { }

void DS18B20Manager::begin() {
    _sensors.begin();
    _sensors.setWaitForConversion(false);
    _slowPolling = true;
    if (_sensors.getAddress(_deviceAddress, 0)) {
        _currentState = State::STOPPED;  // Begin in stopped state
        setResolution(9);
    } else {
        _currentState = State::ERROR;
        DEBUG_PRINTLN("DS18B20 not found!");
    }
}

void DS18B20Manager::startConversion() {
    _lastErrorTime = 0;
    _errorRetryCount = 0;
    _lastUpdateTime = millis();
    _sensors.requestTemperatures();
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
        case State::WAITING_CONVERSION: {
            const uint32_t currentMillis = millis();
            if (currentMillis - _lastUpdateTime >= getConversionDelay()) {
                _currentState = State::READING_TEMP;
                _lastUpdateTime = currentMillis;
            }
            break;
        }

        case State::READING_TEMP: {
            const float temp = _sensors.getTempCByIndex(0);
            if (temp == DEVICE_DISCONNECTED_C) {
                DEBUG_PRINTLN("Error reading temperature!");
                _currentState = State::ERROR;
            } else {
                _lastTemperature = temp;

                if (_currentResolution == 9) {
                    setResolution(12);
                }

                startConversion();
                _currentState = State::WAITING_CONVERSION;
                _lastUpdateTime = millis();
            }
            break;
        }

        case State::ERROR: {
            const uint32_t currentMillis = millis();
            // Only attempt recovery after delay and if under max retries
            if (currentMillis - _lastErrorTime >= _errorRetryDelay && _errorRetryCount < _maxErrorRetries) {
                DEBUG_PRINTLN("Attempting to recover from error...");
                _sensors.begin(); // Reinitialize the sensor
                if (_sensors.getAddress(_deviceAddress, 0)) {
                    setResolution(10); // quick retry
                    _currentState = State::WAITING_CONVERSION;
                    startConversion();
                    _errorRetryCount = 0; // Reset counter on successful recovery
                } else {
                    DEBUG_PRINTLN("Recovery failed. Sensor not found.");
                    _errorRetryCount++;
                    _lastErrorTime = currentMillis;
                    if (_errorRetryCount >= _maxErrorRetries) {
                        DEBUG_PRINTLN("Max retries reached. Stopping polling.");
                        _currentState = State::STOPPED;
                    }
                }
            }
        }

        case State::STOPPED:
            // Do nothing while stopped
            break;
    }
}

void DS18B20Manager::setSlowPolling(bool slowPolling) {
    _slowPolling = slowPolling;
}

int DS18B20Manager::getConversionDelay() const
{
    const int baseDelay = 750 / (1 << (12 - _currentResolution)) + 50; // 50ms for stabilization
    return _slowPolling ? baseDelay + 10000 : baseDelay; // Add 10 seconds if in slow polling mode
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
