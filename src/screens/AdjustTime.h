#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Storage.h"

class AdjustTime : public Screen {
public:
    AdjustTime(DisplayManager* display, InputManager* inputManager);
    void begin(const char* title, const uint8_t startH = 0, const uint8_t startM = 0);
    void beginImpl() override {};
    bool update(bool forceRedraw = false) override;

private:
    void beginImpl(const char* title, const uint8_t startH = 0, const uint8_t startM = 0);

    Storage _storage;
    const char* _title;
    uint8_t _currentDays;
    int8_t _currentHours;
    int8_t _currentMinutes;
    bool _adjustingHours;
    uint8_t _valueY;
    int64_t _oldPosition;

    DisplayManager* display;
    InputManager* inputManager;
    const char* startTime;

    void drawScreen();
};
