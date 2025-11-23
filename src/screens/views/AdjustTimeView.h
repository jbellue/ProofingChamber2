#pragma once

#include "../Screen.h"
#include "DisplayManager.h"
#include "SimpleTime.h"

class AdjustTimeView {
public:
    AdjustTimeView(DisplayManager* display);
    void showTitle(const char* title);
    void drawTime(const SimpleTime& time, uint8_t valueY);
    void drawHighlight(uint8_t selectedItem, uint8_t valueY);
    void drawButtons(uint8_t highlightedButton);
    void sendBuffer();
private:
    DisplayManager* _display;
};
