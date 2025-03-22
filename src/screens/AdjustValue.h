#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Storage.h"

class AdjustValue : public Screen {
public:
AdjustValue(DisplayManager* display, InputManager* inputManager);
    void begin(const char* title, const char* path);
    void beginImpl() override {};
    bool update(bool forceRedraw = false) override;
private:
    Storage _storage;
    const char* _title;
    const char* _path;
    uint8_t _valueY;
    int64_t _oldPosition;
    int _currentValue;
    DisplayManager* display;
    InputManager* inputManager;
    const char* startTime;
    int currentTemp;
    bool isRising;
    bool isIconOn;

    void drawScreen();
    void beginImpl(const char* title, const char* path);
};
