#pragma once

#include "IBaseView.h"
#include <ctime>

class CoolingView : public IBaseView {
public:
    explicit CoolingView(DisplayManager* display): IBaseView(display) {};
    void drawTime(const char* timeBuffer);
};
