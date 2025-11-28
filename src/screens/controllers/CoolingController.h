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
    CoolingView* _view;
    InputManager* _inputManager;
    TemperatureController* _temperatureController;
    AppContext* _ctx;
    time_t _endTime;
    time_t _lastUpdateTime;
    time_t _lastGraphUpdate;
    bool _onCancelButton;
    TimeCalculatorCallback _timeCalculator;
    Screen* _proofingController;
    Screen* _menuScreen;
    Graph _temperatureGraph;
    float _currentTemp;
};
