#pragma once
#include "IBaseView.h"
#include "SimpleTime.h"

class AdjustTimeView : public IBaseView {
public:
    explicit AdjustTimeView(DisplayManager* display) : IBaseView(display) {}
    void drawTime(const SimpleTime& time, uint8_t valueY);
    void drawHighlight(uint8_t selectedItem, uint8_t valueY);
    void drawButtons(const int8_t selectedButton);
    uint8_t start(const char* title, const SimpleTime& time, uint8_t selectedItem, int8_t selectedButton = -1);
};
