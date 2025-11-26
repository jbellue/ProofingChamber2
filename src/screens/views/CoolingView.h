#pragma once

#include "../../Graph.h"
#include "IBaseView.h"
#include <ctime>

class CoolingView : public IBaseView {
public:
    explicit CoolingView(DisplayManager* display): IBaseView(display) {};
    void drawTime(const char* timeBuffer);
    void drawTemperature(const char* tempBuffer);
    void drawIcons(bool on);
    void drawGraph(Graph& graph);
};
