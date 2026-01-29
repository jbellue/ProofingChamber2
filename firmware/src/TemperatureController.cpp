#include "TemperatureController.h"
#include "DebugUtils.h"
#include "services/IStorage.h"
#include "StorageConstants.h"

TemperatureController::TemperatureController(const gpio_num_t heaterPin, const gpio_num_t coolerPin, const gpio_num_t proofingLedPin, const gpio_num_t coolingLedPin)
    : _heaterPin(heaterPin)
    , _coolerPin(coolerPin)
    , _proofingLedPin(proofingLedPin)
    , _coolingLedPin(coolingLedPin)
    , _currentMode(OFF)
    , _storage(nullptr)
    , _lowerLimit(0)
    , _higherLimit(0)
    , _isHeating(false)
    , _isCooling(false)
{
}

void TemperatureController::setStorage(services::IStorage* storage) {
    _storage = storage;
}

void TemperatureController::setDefaultLimits(int8_t lower, int8_t higher) {
    _lowerLimit = lower;
    _higherLimit = higher;
    DEBUG_PRINTLN("Using default temperature limits");
    DEBUG_PRINT("Lower: "); DEBUG_PRINTLN(_lowerLimit);
    DEBUG_PRINT("Higher: "); DEBUG_PRINTLN(_higherLimit);
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
            gpio_set_level(_proofingLedPin, HIGH);
            gpio_set_level(_coolingLedPin, LOW);
            break;
        case COOLING:
            gpio_set_level(_proofingLedPin, LOW);
            gpio_set_level(_coolingLedPin, HIGH);
            break;
        case OFF:
            gpio_set_level(_proofingLedPin, LOW);
            gpio_set_level(_coolingLedPin, LOW);
            break;
    }
}

void TemperatureController::loadTemperatureSettings() {
    switch (_currentMode)
    {
        case HEATING:

            _lowerLimit = _storage->getInt(storage::keys::HOT_LOWER_LIMIT_KEY, storage::defaults::HOT_LOWER_LIMIT_DEFAULT);
            _higherLimit = _storage->getInt(storage::keys::HOT_UPPER_LIMIT_KEY, storage::defaults::HOT_UPPER_LIMIT_DEFAULT);
            break;
        case COOLING:
            _lowerLimit = _storage->getInt(storage::keys::COLD_LOWER_LIMIT_KEY, storage::defaults::COLD_LOWER_LIMIT_DEFAULT);
            _higherLimit = _storage->getInt(storage::keys::COLD_UPPER_LIMIT_KEY, storage::defaults::COLD_UPPER_LIMIT_DEFAULT);
            break;
        case OFF:
            return; // No need to load settings when off
    }

    DEBUG_PRINT("Mode: ");
    DEBUG_PRINTLN(_currentMode == HEATING ? "HEATING" : "COOLING");
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

/**
 * @brief Updates the heating/cooling relays based on current temperature and mode.
 * 
 * This method implements hysteresis control using lower and higher temperature limits.
 * The hysteresis prevents rapid relay switching by creating a temperature range:
 * - In HEATING mode: heater turns ON below lower limit, OFF above higher limit
 * - In COOLING mode: cooler turns ON above higher limit, OFF below lower limit
 * 
 * The gap between lower and higher limits provides the hysteresis band that
 * maintains temperature stability and protects relay lifespan.
 * 
 * @param currentTemp The current temperature reading to compare against limits
 */
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
    gpio_set_level(_heaterPin, on ? HIGH : LOW);
    _isHeating = on;
}

void TemperatureController::turnCooler(bool on) {
    gpio_set_level(_coolerPin, on ? HIGH : LOW);
    _isCooling = on;
}
