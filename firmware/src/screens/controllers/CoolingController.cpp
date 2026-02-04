#include "CoolingController.h"
#include "DebugUtils.h"
#include "../views/CoolingView.h"
#include "../../ScreensManager.h"

CoolingController::CoolingController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr), _temperatureController(nullptr),
    _proofingController(nullptr), _menuScreen(nullptr) {}

void CoolingController::beginImpl() {
    initializeInputManager();
    
    AppContext* ctx = getContext();
    if (ctx) {
        if (!_temperatureController) _temperatureController = ctx->tempController;
        if (!_view) _view = ctx->coolingView;
    }
    
    getInputManager()->slowTemperaturePolling(false);
    _endTime = _timeCalculator ? _timeCalculator() : 0;
    _lastUpdateTime = 0;
    _lastGraphUpdate = 0;
    _onCancelButton = true;
    _temperatureController->setMode(ITemperatureController::COOLING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _view->start(_endTime, _onCancelButton, _temperatureGraph);
}

bool CoolingController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    if (shouldRedraw) {
        _endTime = _timeCalculator ? _timeCalculator() : _endTime;
    }
    bool timesUp = now >= _endTime;
    if (inputManager->isButtonPressed() || timesUp) {
        inputManager->slowTemperaturePolling(true);
        _temperatureController->setMode(ITemperatureController::OFF);
        _endTime = 0; // Reset when exiting
        bool goingToProofScreen = !_onCancelButton || timesUp;
        BaseController* nextScreen = goingToProofScreen ? _proofingController : _menuScreen;
        setNextScreen(nextScreen);
        if (goingToProofScreen && _proofingController) {
            _proofingController->setNextScreen(_menuScreen);
        }
        if (nextScreen) nextScreen->begin();
        return false;
    }
    if (difftime(now, _lastUpdateTime) >= 1) {
        const float currentTemp = inputManager->getTemperature();
        _temperatureGraph.addValueToAverage(currentTemp);
        shouldRedraw |= _view->drawTemperature(currentTemp);
        _temperatureController->update(currentTemp);
        shouldRedraw |= _view->drawTime(difftime(_endTime, now));
        _lastUpdateTime = now;
        if (difftime(now, _lastGraphUpdate) >= 10.0) {
            _temperatureGraph.commitAverage(currentTemp);
            _lastGraphUpdate = now;
            _view->drawGraph(_temperatureGraph);
            shouldRedraw = true;
        }
    }

    shouldRedraw |= _view->drawIcons(OptionalBool(_temperatureController->isCooling()));

    auto encoderDirection = inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        shouldRedraw = true;
    }

    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}


void CoolingController::prepare(TimeCalculatorCallback callback, BaseController* proofingController, BaseController* menuScreen) {
    _timeCalculator = callback;
    _proofingController = proofingController;
    _menuScreen = menuScreen;
}

void CoolingController::startCooling(time_t endTime) {
    AppContext* ctx = getContext();
    if (!ctx || !ctx->tempController) return;
    
    _temperatureController = ctx->tempController;
    _endTime = endTime;
    _lastUpdateTime = 0;
    _lastGraphUpdate = 0;
    
    _temperatureController->setMode(ITemperatureController::COOLING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    
    DEBUG_PRINTLN("[CoolingController] Started cooling from web API");
    
    // Navigate display to show cooling screen
    if (ctx->screens) {
        ctx->screens->setActiveScreen(this);
    }
}

void CoolingController::startCoolingWithDelay(int hours) {
    struct tm now;
    getLocalTime(&now);
    time_t currentTime = mktime(&now);
    time_t endTime = currentTime + (hours * 3600);
    startCooling(endTime);
}

void CoolingController::stopCooling() {
    AppContext* ctx = getContext();
    if (_temperatureController) {
        _temperatureController->setMode(ITemperatureController::OFF);
    }
    _endTime = 0;
    
    DEBUG_PRINTLN("[CoolingController] Stopped cooling from web API");
    
    // Navigate back to menu
    if (ctx && ctx->screens && ctx->menu) {
        ctx->screens->setActiveScreen(ctx->menu);
    }
}
