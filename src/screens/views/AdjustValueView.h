#pragma once
#include "DisplayManager.h"

class AdjustValueView {
public:
    AdjustValueView(DisplayManager* display);
    void drawValue(int value, uint8_t valueY);
    void drawButton(const char* label, bool selected);
    void clear();
    void setFont(const uint8_t* font);
    void sendBuffer();
    int getDisplayWidth() const;
    uint8_t getUTF8Width(const char* str) const;
    uint8_t getAscent() const;
private:
    DisplayManager* _display;
};
