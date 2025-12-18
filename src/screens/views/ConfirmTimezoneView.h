#pragma once
#include "IBaseView.h"

class ConfirmTimezoneView : public IBaseView {
public:
    explicit ConfirmTimezoneView(DisplayManager* display) : IBaseView(display) {}
    void start();
    void setTimezone(const char* continentName, const char* timezoneName);
    void drawButtons(bool onCancelSelected);

private:
    const char* _continentName = nullptr;
    const char* _timezoneName = nullptr;
};
