#pragma once

#include "screens/BaseController.h"

class ScreensManager {
public:
    ScreensManager();
    void setActiveScreen(BaseController* screen);
    void update();
    bool isScreenActive() const;
    BaseController* getActiveScreen() const { return activeScreen; }

private:
    BaseController* activeScreen;
};
