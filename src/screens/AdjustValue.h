#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"

class AdjustValue : public Screen {
public:
AdjustValue(DisplayManager* display, InputManager* inputManager);
    void begin(const char* title, const char* path);
    void beginImpl() override {};
    bool update(bool forceRedraw = false) override;
private:
    void drawValue();
    void drawButton();
    void beginImpl(const char* title, const char* path);

    const char* _title;
    const char* _path;
    uint8_t _valueY;
    int _currentValue;
    DisplayManager* _display;
    InputManager* _inputManager;
    const char* _startTime;
};
