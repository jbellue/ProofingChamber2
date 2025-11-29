#include "TemperatureController.h"
#include "DebugUtils.h"
#include "services/IStorage.h"

static services::IStorage* _globalStorage = nullptr; // fallback pointer set by constructor or setter

TemperatureController::TemperatureController(uint8_t heaterPin, uint8_t coolerPin, uint8_t proofingLedPin, uint8_t coolingLedPin)
    : _heaterPin(heaterPin)
    , _coolerPin(coolerPin)
    , _proofingLedPin(proofingLedPin)
    , _coolingLedPin(coolingLedPin)
    , _currentMode(OFF)
    , _targetTemp(0)
    , _lowerLimit(0)
    , _higherLimit(0)
    , _isHeating(false)
    , _isCooling(false)
{
}

void TemperatureController::setStorage(services::IStorage* storage) {
    _globalStorage = storage;
}

void TemperatureController::begin() {
    pinMode(_heaterPin, OUTPUT);
    pinMode(_coolerPin, OUTPUT);
    pinMode(_proofingLedPin, OUTPUT);
    pinMode(_coolingLedPin, OUTPUT);
    turnHeater(false);
    turnCooler(false);
}

void TemperatureController::setMode(Mode mode) {
    if (_currentMode == mode) return;
    
    _currentMode = mode;
    loadTemperatureSettings();

    // Safety: turn off both relays when changing modes
    turnHeater(false);
    turnCooler(false);

    // Control LEDs based on mode
    switch (_currentMode) {
        case HEATING:
            digitalWrite(_proofingLedPin, HIGH);
            digitalWrite(_coolingLedPin, LOW);
            break;
        case COOLING:
            digitalWrite(_proofingLedPin, LOW);
            digitalWrite(_coolingLedPin, HIGH);
            break;
        case OFF:
            digitalWrite(_proofingLedPin, LOW);
            digitalWrite(_coolingLedPin, LOW);
            break;
    }
}

void TemperatureController::loadTemperatureSettings() {
    const char* targetFile;
    const char* lowerFile;
    const char* higherFile;

    switch (_currentMode)
    {
        case HEATING:
            targetFile = "/hot/target_temp.txt";
            lowerFile = "/hot/lower_limit.txt";
            higherFile = "/hot/higher_limit.txt";
            break;
        case COOLING:
            targetFile = "/cold/target_temp.txt";
            lowerFile = "/cold/lower_limit.txt";
            higherFile = "/cold/higher_limit.txt";
            break;
        case OFF:
            return; // No need to load settings when off
    }

    if (_globalStorage) {
        _targetTemp = _globalStorage->readInt(targetFile);
        _lowerLimit = _globalStorage->readInt(lowerFile);
        _higherLimit = _globalStorage->readInt(higherFile);
    }

    DEBUG_PRINT("Mode: ");
    DEBUG_PRINTLN(_currentMode == HEATING ? "HEATING" : "COOLING");
    DEBUG_PRINT("Target: ");
    DEBUG_PRINTLN(_targetTemp);
    DEBUG_PRINT("Lower: ");
    DEBUG_PRINTLN(_lowerLimit);
    DEBUG_PRINT("Higher: ");
    DEBUG_PRINTLN(_higherLimit);
}

void TemperatureController::update(float currentTemp) {
    if (_currentMode == OFF) {
        turnHeater(false);
        turnCooler(false);
        return;
    }

    updateRelays(currentTemp);
}

void TemperatureController::updateRelays(float currentTemp) {
    switch (_currentMode) {
        case HEATING:
            if (currentTemp < _lowerLimit) {
                DEBUG_PRINTLN("Turning the heater ON");
                turnHeater(true);
                turnCooler(false);
            } else if (currentTemp > _higherLimit) {
                DEBUG_PRINTLN("Turning the heater OFF");
                turnHeater(false);
                turnCooler(false);
            }
            break;
        case COOLING:
            if (currentTemp > _higherLimit) {
                DEBUG_PRINTLN("Turning the cooler ON");
                turnCooler(true);
                turnHeater(false);
            } else if (currentTemp < _lowerLimit) {
                DEBUG_PRINTLN("Turning the cooler OFF");
                turnCooler(false);
                turnHeater(false);
            }
            break;
        case OFF: // No action needed
            break;
    }
}

TemperatureController::Mode TemperatureController::getMode() const {
    return _currentMode;
}

bool TemperatureController::isHeating() const {
    return _isHeating;
}

bool TemperatureController::isCooling() const {
    return _isCooling;
}

void TemperatureController::turnHeater(bool on) {
    digitalWrite(_heaterPin, on ? HIGH : LOW);
    _isHeating = on;
}

void TemperatureController::turnCooler(bool on) {
    digitalWrite(_coolerPin, on ? HIGH : LOW);
    _isCooling = on;
}
