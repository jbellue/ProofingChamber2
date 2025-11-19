#include "TemperatureController.h"
#include "DebugUtils.h"
#include "services/IStorage.h"

static services::IStorage* _globalStorage = nullptr; // fallback pointer set by constructor or setter

TemperatureController::TemperatureController(uint8_t heaterPin, uint8_t coolerPin) 
    : _heaterPin(heaterPin)
    , _coolerPin(coolerPin)
    , _currentMode(OFF)
    , _targetTemp(0)
    , _lowerLimit(0)
    , _higherLimit(0)
{
}

void TemperatureController::setStorage(services::IStorage* storage) {
    _globalStorage = storage;
}

void TemperatureController::begin() {
    pinMode(_heaterPin, OUTPUT);
    pinMode(_coolerPin, OUTPUT);
    digitalWrite(_heaterPin, LOW);
    digitalWrite(_coolerPin, LOW);
}

void TemperatureController::setMode(Mode mode) {
    if (_currentMode == mode) return;
    
    _currentMode = mode;
    loadTemperatureSettings();
    
    // Safety: turn off both relays when changing modes
    digitalWrite(_heaterPin, LOW);
    digitalWrite(_coolerPin, LOW);
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
        digitalWrite(_heaterPin, LOW);
        digitalWrite(_coolerPin, LOW);
        return;
    }

    updateRelays(currentTemp);
}

void TemperatureController::updateRelays(float currentTemp) {
    DEBUG_PRINT("Mode: ");
    DEBUG_PRINT(_currentMode == HEATING ? "HEATING" : "COOLING");
    DEBUG_PRINT(" - Current temperature: ");
    DEBUG_PRINT(currentTemp);
    DEBUG_PRINT(" - Target: ");
    DEBUG_PRINT(_targetTemp);
    DEBUG_PRINT(" - Lower: ");
    DEBUG_PRINT(_lowerLimit);
    DEBUG_PRINT(" - Higher: ");
    DEBUG_PRINTLN(_higherLimit);
    switch (_currentMode) {
        case HEATING:
            if (currentTemp < _lowerLimit) {
                DEBUG_PRINTLN("Turning the heater ON");
                digitalWrite(_heaterPin, HIGH);  // Turn heater on
                digitalWrite(_coolerPin, LOW);   // Make sure cooler is off
            } else if (currentTemp > _higherLimit) {
                DEBUG_PRINTLN("Turning the heater OFF");
                digitalWrite(_heaterPin, LOW);   // Turn heater off
            }
            break;
        case COOLING:
            if (currentTemp > _higherLimit) {
                DEBUG_PRINTLN("Turning the cooler ON");
                digitalWrite(_coolerPin, HIGH);  // Turn cooler on
                digitalWrite(_heaterPin, LOW);   // Make sure heater is off
            } else if (currentTemp < _lowerLimit) {
                DEBUG_PRINTLN("Turning the cooler OFF");
                digitalWrite(_coolerPin, LOW);   // Turn cooler off
            }
            break;
        case OFF: // No action needed
            break;
    }
}

TemperatureController::Mode TemperatureController::getMode() const {
    return _currentMode;
}