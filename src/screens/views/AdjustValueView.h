#pragma once
#include "IBaseView.h"

class AdjustValueView : public IBaseView {
public:
    explicit AdjustValueView(DisplayManager* display) : IBaseView(display) {}
    bool drawValue(int value, uint8_t valueY);
    void drawButtons();
    uint8_t start(const char* title, const int value);
private:
    void reset();
    int _lastValueDrawn = INT32_MIN;
};
