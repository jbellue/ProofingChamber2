#pragma once
#include "IBaseView.h"

class SetTimezoneView : public IBaseView {
public:
    SetTimezoneView(DisplayManager* display) : IBaseView(display) {}
    void showInitialPrompt();
};
