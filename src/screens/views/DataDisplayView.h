#pragma once
#include "IBaseView.h"
#include "../../OptionalBool.h"

class DataDisplayView : public IBaseView {
public:
    explicit DataDisplayView(DisplayManager* display) : IBaseView(display) {}
    void start();
    void reset();
    bool drawTemperature(float temperatureC);
    bool drawTime(const tm &timeinfo);
    void drawButtons();
private:
    void drawTitle();
    float _lastTemperature = -273.15;
    int _lastMinute = -1;

};