#pragma once
#include "IBaseView.h"
#include "../../Graph.h"

class ProofingView : public IBaseView {
public:
    explicit ProofingView(DisplayManager* display) : IBaseView(display) {}
    void drawTime(const time_t diffSeconds);
    void drawTemperature(const float currentTemp);
    void drawIcons(bool on);
    void drawGraph(Graph& graph);
    void drawButtons();
};
