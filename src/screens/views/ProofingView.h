#pragma once
#include "IBaseView.h"
#include "../../Graph.h"
#include "../../icons.h"
#include "../../OptionalBool.h"

class ProofingView : public IBaseView {
public:
    explicit ProofingView(DisplayManager* display) : IBaseView(display) {}
    bool drawTime(const time_t diffSeconds);
    bool drawTemperature(const float currentTemp);
    bool drawIcons(OptionalBool iconState);
    void drawGraph(Graph& graph);
    void reset();
    void start(float currentTemp, Graph& graph);
private:
    float _lastTempDrawn = -257.0;
    OptionalBool _lastIconState;
    time_t _lastTimeDrawn = -1000;
};
