#pragma once
#include "IBaseView.h"
#include "../../Graph.h"

class ProofingView : public IBaseView {
public:
    explicit ProofingView(DisplayManager* display) : IBaseView(display) {}
    void drawTitle();
    void drawTime(const char* timeBuffer);
    void drawTemperature(const char* tempBuffer);
    void drawIcons(bool on);
    void drawButtons();
    void drawGraph(Graph& graph);
};
