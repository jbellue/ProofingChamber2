#pragma once
#include "IBaseView.h"
#include "SimpleTime.h"

class AdjustTimeView : public IBaseView {
public:
    AdjustTimeView(DisplayManager* display) : IBaseView(display) {}
    void drawTime(const SimpleTime& time, uint8_t valueY);
    void drawHighlight(uint8_t selectedItem, uint8_t valueY);
    void drawButtons(uint8_t highlightedButton);
};
