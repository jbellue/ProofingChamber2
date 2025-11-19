#pragma once
#include "../../DisplayManager.h"
#include "../../Graph.h"

class ProofingView {
public:
    explicit ProofingView(DisplayManager* display);

    void clear();
    void drawTitle();
    void drawTime(const char* timeBuffer);
    void drawTemperature(const char* tempBuffer);
    void drawIcons(bool on);
    void drawButtons();
    void drawGraph(Graph& graph);
    void sendBuffer();

private:
    DisplayManager* _display;
};
