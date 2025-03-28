#pragma once

#include "Screen.h"
#include "DisplayManager.h"

class WiFiReset : public Screen {
public:
WiFiReset(DisplayManager* display, InputManager* inputManager);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    bool _onCancelButton;
    void drawScreen();
    void beginImpl() override;
};
