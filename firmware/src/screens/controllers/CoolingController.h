#pragma once
#include "../../AppContextDecl.h"
#include "../BaseController.h"
#include "../views/CoolingView.h"
#include "../../IInputManager.h"
#include "../../ITemperatureController.h"
#include <ctime>

class CoolingController : public BaseController {
public:
    using TimeCalculatorCallback = time_t (*)();

    CoolingController(AppContext* ctx);
    void beginImpl() override;
    bool update(bool forceRedraw = false) override;
    void prepare(TimeCalculatorCallback callback, BaseController* proofingController, BaseController* menuScreen);
    
    const char* getScreenName() const override { return "Cooling"; }
    
    // Getters for web interface
    time_t getEndTime() const { return _endTime; }
    bool isActive() const { return _endTime != 0; }
    
    // Direct API methods for web interface
    void startCooling(time_t endTime);
    void startCoolingWithDelay(int hours);
    void stopCooling();

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
