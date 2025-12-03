#pragma once

#include "../../Graph.h"
#include "IBaseView.h"
#include "../../OptionalBool.h"
#include <ctime>

class CoolingView : public IBaseView {
public:
    explicit CoolingView(DisplayManager* display): IBaseView(display) {};
    bool drawTime(const int remainingSeconds);
    bool drawTemperature(const float currentTemp);
    bool drawIcons(OptionalBool iconState);
    void drawButtons(bool onCancelSelected);
    void drawGraph(Graph& graph);
    void start(const time_t endTime, bool onCancelSelected, Graph& graph);
private:
    int _lastRemainingSeconds = -1;
    OptionalBool _lastIconState;
    float _lastTemperature = -273.15; // Initialize with a value below absolute zero to ensure first draw
    uint8_t _timeWidth; // Width of the time string for clearing
    void formatTimeString(char* buffer, const size_t bufferSize, const int remainingSeconds);
    void reset();
    void drawTitle(const time_t endTime);
};
