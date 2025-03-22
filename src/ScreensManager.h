#pragma once

#include "screens/Screen.h"

class ScreensManager {
public:
    ScreensManager();
    void setActiveScreen(Screen* screen);
    void update();
    bool isScreenActive() const;
    Screen* getActiveScreen() const { return activeScreen; }

private:
    Screen* activeScreen;
};
