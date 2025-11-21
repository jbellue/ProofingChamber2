#pragma once

#include "Screen.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "TemperatureController.h"
#include <functional>
#include <ctime>
#include "AppContextDecl.h"

class CoolingScreen : public Screen {
public:
    // Define a callback type that returns the end time
    using TimeCalculatorCallback = std::function<time_t()>;

    CoolingScreen(AppContext* ctx);
    void begin(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen);
    void beginImpl() override;
    // Prepare the parameters so begin() can be called later by ScreensManager
    void prepare(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen);
    bool update(bool forceRedraw = false) override;

private:
    DisplayManager* _display;
    InputManager* _inputManager;
    time_t _endTime;
    Screen* _ProofingController;
    Screen* _menuScreen;
    bool _onCancelButton;
    TimeCalculatorCallback _timeCalculator;
    time_t _lastUpdateTime;
    TemperatureController* _temperatureController;
    time_t _lastTemperatureUpdate;
    float _previousTemp;
    AppContext* _ctx;

    void drawScreen();
    void beginImpl(TimeCalculatorCallback callback, Screen* ProofingController, Screen* menuScreen);

};
