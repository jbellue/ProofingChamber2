#pragma once

#include "../../Graph.h"
#include "IBaseView.h"
#include <ctime>

class CoolingView : public IBaseView {
public:
    explicit CoolingView(DisplayManager* display): IBaseView(display) {};
    bool drawTime(const int remainingSeconds);
    void drawTemperature(const float currentTemp);
    bool drawIcons(bool on, bool force = false);
    void drawGraph(Graph& graph);
private:
    int _lastRemainingSeconds = -1;
    bool _lastIconState = false;
};
