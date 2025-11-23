#pragma once

#include "../Screen.h"
#include "DisplayManager.h"

class WiFiResetView {
public:
    WiFiResetView(DisplayManager* display);
    void showInitialPrompt();
    void showResetMessage();
    void drawButtons(bool onCancelButton);
private:
    DisplayManager* _display;
};
