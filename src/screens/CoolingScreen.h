#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include <ctime>

class CoolingScreen : public Screen {
public:
    CoolingScreen(DisplayManager* display, InputManager* inputManager);
    void begin(time_t endTime, Screen* proofingScreen, Screen* menuScreen);
    void beginImpl() override {}
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _endTime;
    Screen* _proofingScreen;
    Screen* _menuScreen;
    int64_t _oldPosition;
    bool _onCancelButton;

    void drawScreen();
    void beginImpl(time_t endTime, Screen* proofingScreen, Screen* menuScreen);

};
