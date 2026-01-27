#pragma once
#include "IBaseView.h"

class PowerOffView : public IBaseView {
public:
    explicit PowerOffView(DisplayManager* display) : IBaseView(display) {}
    void start();
    void drawButtons(bool onCancelSelected);
};
