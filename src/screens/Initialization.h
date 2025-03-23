#pragma once

#include "Screen.h"
#include "DisplayManager.h"

class Initialization : public Screen {
public:
    Initialization(DisplayManager* display);
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    void drawScreen();
    void beginImpl() override;
};
