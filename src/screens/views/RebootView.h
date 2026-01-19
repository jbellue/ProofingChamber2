#pragma once
#include "IBaseView.h"

class RebootView : public IBaseView {
public:
    explicit RebootView(DisplayManager* display) : IBaseView(display) {}
    void start();
    void drawButtons(bool onCancelSelected);
};
