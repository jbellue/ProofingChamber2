#pragma once
#include "AppContextDecl.h"
#include "../BaseController.h"
#include "../views/CoolingView.h"
#include "IInputManager.h"
#include "ITemperatureController.h"
#include <ctime>

class CoolingController : public BaseController {
public:
    using TimeCalculatorCallback = time_t (*)();

    CoolingController(AppContext* ctx);
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;
    void prepare(TimeCalculatorCallback callback, BaseController* proofingController, BaseController* menuScreen);

private:
    CoolingView* _view;
    ITemperatureController* _temperatureController;
    time_t _endTime;
    time_t _lastUpdateTime;
    time_t _lastGraphUpdate;
    bool _onCancelButton;
    TimeCalculatorCallback _timeCalculator;
    BaseController* _proofingController;
    BaseController* _menuScreen;
    Graph _temperatureGraph;
    float _currentTemp;
};
