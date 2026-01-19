#pragma once
#include "IBaseView.h"

class WiFiResetView : public IBaseView {
public:
    explicit WiFiResetView(DisplayManager* display) : IBaseView(display) {}
    void showResetMessage();
    void start();
};
