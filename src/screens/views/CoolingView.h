#pragma once

#include "DisplayManager.h"
#include <ctime>

class CoolingView {
public:
    CoolingView(DisplayManager* display);
    ~CoolingView();
    void clear();
    void drawTitle(const char* title);
    void drawButtons(const char* startLabel, const char* cancelLabel, int selectedButton);
    void drawTime(const char* timeBuffer);
    void sendBuffer();
    int getDisplayWidth() const;
    uint8_t getUTF8Width(const char* str) const;
private:
    DisplayManager* _display;
};
