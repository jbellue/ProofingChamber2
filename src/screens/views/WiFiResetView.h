#pragma once
#include "IBaseView.h"

class WiFiResetView : public IBaseView {
public:
    WiFiResetView(DisplayManager* display) : IBaseView(display) {}
    void showInitialPrompt();
    void showResetMessage();
};
