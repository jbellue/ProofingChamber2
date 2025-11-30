#pragma once
#include "IBaseView.h"

class AdjustValueView : public IBaseView {
public:
    explicit AdjustValueView(DisplayManager* display) : IBaseView(display) {}
    void drawValue(int value, uint8_t valueY);
    void drawButtons();
};
