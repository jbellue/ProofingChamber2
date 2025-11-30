#pragma once
#include "IBaseView.h"

class SetTimezoneView : public IBaseView {
public:
    explicit SetTimezoneView(DisplayManager* display) : IBaseView(display) {}
    void start();
};
