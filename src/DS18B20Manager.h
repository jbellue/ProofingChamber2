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

    int getConversionDelay() const;
    void setResolution(uint8_t bits);
    void startConversion();
    void handleState();
};