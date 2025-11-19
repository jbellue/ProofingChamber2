#pragma once
#include "../../DisplayManager.h"

class RebootView {
public:
    explicit RebootView(DisplayManager* display);
    void showTitle();
    void drawButtons(bool onCancel);

private:
    DisplayManager* _display;
};
