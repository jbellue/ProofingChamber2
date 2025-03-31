#ifndef PROOFING_SCREEN_H
#define PROOFING_SCREEN_H

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Graph.h"

class ProofingScreen : public Screen {
public:
    ProofingScreen(DisplayManager* display, InputManager* inputManager);
    void beginImpl() override;
    void begin();
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _startTime;
    time_t _lastGraphUpdate;
    time_t _lastTemperatureUpdate;
    time_t _previousDiffSeconds;
    float _previousTemp;
    float _currentTemp;
    bool _isIconOn;
    Graph _temperatureGraph;

    void drawTime();
    void drawTemperature();
    void drawIcons();
    void drawButtons();
    void drawGraph();
};

#endif
