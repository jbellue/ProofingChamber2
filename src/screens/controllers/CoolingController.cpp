#include "CoolingController.h"
#include "DebugUtils.h"

CoolingController::CoolingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _temperatureController(nullptr),
    _ctx(ctx), _proofingController(nullptr), _menuScreen(nullptr) {}

void CoolingController::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_temperatureController) _temperatureController = _ctx->tempController;
        if (!_view && _ctx->display) {
            _view = new CoolingView(_ctx->display);
        }
    }
    if (_inputManager) {
        _inputManager->resetEncoderPosition();
        _inputManager->slowTemperaturePolling(false);
    }
    _endTime = _timeCalculator ? _timeCalculator() : 0;
    _lastUpdateTime = 0;
    _lastGraphUpdate = 0;
    _previousTemp = 200.0f;
    _onCancelButton = true;
    if (_temperatureController) _temperatureController->setMode(TemperatureController::COOLING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _currentTemp = _inputManager ? _inputManager->getTemperature() : 0.0f;
    if (_view) {
        _view->clear();
        const tm* tm_end = localtime(&_endTime);
        char timeBuffer[34] = {'\0'};
        snprintf(timeBuffer, sizeof(timeBuffer), "D\xC3\xA9marrage de la\npousse \xC3\xA0 %d:%02d", tm_end->tm_hour, tm_end->tm_min);
        _view->drawTitle(timeBuffer);
        _view->drawButtons("D\xC3\xA9marrer", "Annuler", _onCancelButton ? 1 : 0);
        _view->drawGraph(_temperatureGraph);
    }
}

bool CoolingController::update(bool shouldRedraw) {
    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);

    bool isButtonPressed = _inputManager ? _inputManager->isButtonPressed() : false;
    if (shouldRedraw) {
        _endTime = _timeCalculator ? _timeCalculator() : _endTime;
    }
    bool timesUp = now >= _endTime;
    if (isButtonPressed || timesUp) {
        if (_inputManager) _inputManager->slowTemperaturePolling(true);
        if (_temperatureController) _temperatureController->setMode(TemperatureController::OFF);
        bool goingToProofScreen = !_onCancelButton || timesUp;
        Screen* nextScreen = goingToProofScreen ? _proofingController : _menuScreen;
        setNextScreen(nextScreen);
        if (goingToProofScreen && _proofingController) {
            _proofingController->setNextScreen(_menuScreen);
        }
        if (nextScreen) nextScreen->begin();
        return false;
    }
    if (_inputManager && difftime(now, _lastUpdateTime) >= 1) {
        _currentTemp = _inputManager->getTemperature();
        if (abs(_currentTemp - _previousTemp) >= 0.1) {
            _temperatureGraph.addValueToAverage(_currentTemp);
            _previousTemp = _currentTemp;
            _view->drawTemperature(_currentTemp);
            shouldRedraw = true;
            if (_temperatureController) _temperatureController->update(_currentTemp);
        }
        shouldRedraw &= _view->drawTime(difftime(_endTime, now));
        _lastUpdateTime = now;
    }

    shouldRedraw &= _view->drawIcons((_temperatureController && _temperatureController->isCooling()));

    if (difftime(now, _lastGraphUpdate) >= 10.0) {
        _temperatureGraph.commitAverage(_currentTemp);
        _lastGraphUpdate = now;
        _view->drawGraph(_temperatureGraph);
        shouldRedraw = true;
    }

    auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : InputManager::EncoderDirection::None;
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons("D\xC3\xA9marrer", "Annuler", _onCancelButton ? 1 : 0);
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
