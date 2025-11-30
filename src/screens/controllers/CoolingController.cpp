#include "CoolingController.h"
#include "DebugUtils.h"
#include "../views/CoolingView.h"

CoolingController::CoolingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _temperatureController(nullptr),
    _ctx(ctx), _proofingController(nullptr), _menuScreen(nullptr) {}

void CoolingController::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_temperatureController) _temperatureController = _ctx->tempController;
        if (!_view) _view = _ctx->coolingView;
    }
    if (_inputManager) {
        _inputManager->resetEncoderPosition();
        _inputManager->slowTemperaturePolling(false);
    }
    _endTime = _timeCalculator ? _timeCalculator() : 0;
    _lastUpdateTime = 0;
    _lastGraphUpdate = 0;
    _onCancelButton = true;
    if (_temperatureController) _temperatureController->setMode(TemperatureController::COOLING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _view->clear();
    _view->drawTitle(_endTime);
    _view->drawButtons(_onCancelButton);
    _view->drawGraph(_temperatureGraph);
}

bool CoolingController::update(bool shouldRedraw) {
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    if (shouldRedraw) {
        _endTime = _timeCalculator ? _timeCalculator() : _endTime;
    }
    bool timesUp = now >= _endTime;
    if (_inputManager->isButtonPressed() || timesUp) {
        _inputManager->slowTemperaturePolling(true);
        _temperatureController->setMode(TemperatureController::OFF);
        bool goingToProofScreen = !_onCancelButton || timesUp;
        Screen* nextScreen = goingToProofScreen ? _proofingController : _menuScreen;
        setNextScreen(nextScreen);
        if (goingToProofScreen && _proofingController) {
            _proofingController->setNextScreen(_menuScreen);
        }
        if (nextScreen) nextScreen->begin();
        _view->reset();
        return false;
    }
    if (difftime(now, _lastUpdateTime) >= 1) {
        const float currentTemp = _inputManager->getTemperature();
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

    shouldRedraw |= _view->drawIcons(_temperatureController->isCooling() ? IconState::On : IconState::Off);

    auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : InputManager::EncoderDirection::None;
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        shouldRedraw = true;
    }

    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}


void CoolingController::prepare(TimeCalculatorCallback callback, Screen* proofingController, Screen* menuScreen) {
    _timeCalculator = callback;
    _proofingController = proofingController;
    _menuScreen = menuScreen;
}
