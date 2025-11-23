#pragma once

#include "../Screen.h"
#include "DisplayManager.h"

class SetTimezoneView {
public:
    SetTimezoneView(DisplayManager* display);
    void showInitialPrompt();
    void drawButton();
private:
    DisplayManager* _display;
};
