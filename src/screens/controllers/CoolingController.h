#pragma once
#include "AppContextDecl.h"
#include "../Screen.h"
#include "../views/CoolingView.h"
#include "InputManager.h"
#include "TemperatureController.h"
#include <ctime>

class CoolingController : public Screen {
public:
    using TimeCalculatorCallback = std::function<time_t()>;

    CoolingController(AppContext* ctx);
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;
    void prepare(TimeCalculatorCallback callback, Screen* proofingController, Screen* menuScreen);

private:
    char _lastDisplayedTime[34] = {0};
    CoolingView* _view;
    InputManager* _inputManager;
    TemperatureController* _temperatureController;
    AppContext* _ctx;
    time_t _endTime;
    time_t _lastUpdateTime;
    float _previousTemp;
    bool _onCancelButton;
    bool _isIconOn;
    bool _wasIconOn;
    TimeCalculatorCallback _timeCalculator;
    Screen* _proofingController;
    Screen* _menuScreen;
    Graph _temperatureGraph;
    float _currentTemp;
    void drawTemperature();
    void drawScreen();
};
