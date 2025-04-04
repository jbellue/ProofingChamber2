#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "TemperatureController.h"
#include <functional>
#include <ctime>

class CoolingScreen : public Screen {
public:
    // Define a callback type that returns the end time
    using TimeCalculatorCallback = std::function<time_t()>;

    CoolingScreen(DisplayManager* display, InputManager* inputManager, TemperatureController* temperatureController);
    void begin(TimeCalculatorCallback callback, Screen* proofingScreen, Screen* menuScreen);
    void beginImpl() override {}
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _endTime;
    Screen* _proofingScreen;
    Screen* _menuScreen;
    bool _onCancelButton;
    TimeCalculatorCallback _timeCalculator;
    time_t _lastUpdateTime;
    TemperatureController* _temperatureController;
    time_t _lastTemperatureUpdate;
    float _previousTemp;

    void drawScreen();
    void beginImpl(TimeCalculatorCallback callback, Screen* proofingScreen, Screen* menuScreen);

};
