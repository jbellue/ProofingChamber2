#pragma once

#include "../../Graph.h"
#include "IBaseView.h"
#include "../../icons.h"
#include <ctime>

class CoolingView : public IBaseView {
public:
    explicit CoolingView(DisplayManager* display): IBaseView(display) {};
    bool drawTime(const int remainingSeconds);
    bool drawTemperature(const float currentTemp);
    bool drawIcons(IconState iconState);
    void drawButtons(bool onCancelSelected);
    void reset();
    void drawGraph(Graph& graph);
private:
    int _lastRemainingSeconds = -1;
    IconState _lastIconState = IconState::Unset;
    float _lastTemperature = -273.15; // Initialize with a value below absolute zero to ensure first draw
    uint8_t _timeWidth; // Width of the time string for clearing
    void formatTimeString(char* buffer, const size_t bufferSize, const int remainingSeconds);
};
