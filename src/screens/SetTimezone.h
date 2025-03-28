#pragma once

#include "Screen.h"
#include "DisplayManager.h"

class SetTimezone : public Screen {
public:
    SetTimezone(DisplayManager* display, InputManager* inputManager);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    void drawScreen();
    void beginImpl() override;
};
