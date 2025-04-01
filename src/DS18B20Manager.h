#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Manager {
public:
    enum class State {
        WAITING_CONVERSION,
        READING_TEMP,
        ERROR,
        STOPPED
    };

    DS18B20Manager(const uint8_t oneWirePin);
    void begin();
    void update();
    float getTemperature() const;
    void startPolling();
    void stopPolling();

private:
    OneWire _oneWire;
    DallasTemperature _sensors;
    DeviceAddress _deviceAddress;
    float _lastTemperature;
    uint8_t _currentResolution;
    State _currentState;
    unsigned long _lastUpdateTime;

    const uint32_t _errorRetryDelay = 1500; // Delay in milliseconds for error retry
    uint32_t _lastErrorTime;
    uint8_t _errorRetryCount;
    const uint8_t _maxErrorRetries = 3;

    int getConversionDelay() const;
    void setResolution(uint8_t bits);
    void startConversion();
    void handleState();
};