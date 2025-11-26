#include "CoolingController.h"
#include "DebugUtils.h"

CoolingController::CoolingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _temperatureController(nullptr),
    _ctx(ctx), _endTime(0), _lastUpdateTime(0), _previousTemp(200.0f),
    _onCancelButton(true), _isIconOn(true), _wasIconOn(false),
    _proofingController(nullptr), _menuScreen(nullptr) {}

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
    _previousTemp = 200.0f;
    _onCancelButton = true;
    if (_temperatureController) _temperatureController->setMode(TemperatureController::COOLING);
    if (_view) _view->clear();
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _currentTemp = _inputManager ? _inputManager->getTemperature() : 0.0f;
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
        const float currentTemp = _inputManager->getTemperature();
        if (abs(currentTemp - _previousTemp) > 0.2) {
            _temperatureGraph.addValueToAverage(currentTemp);
            _previousTemp = currentTemp;
            _currentTemp = currentTemp;
            drawTemperature();
            shouldRedraw = true;
            if (_temperatureController) _temperatureController->update(currentTemp);
        }
        // Check if displayed time has changed
        char newTimeBuffer[34] = {0};
        int remainingSeconds = difftime(_endTime, now);
        if (remainingSeconds < 60) {
            snprintf(newTimeBuffer, sizeof(newTimeBuffer), "dans <1m");
        } else if (remainingSeconds >= 3600) {
            snprintf(newTimeBuffer, sizeof(newTimeBuffer), "dans %dh%02dm",
                remainingSeconds / 3600, (remainingSeconds % 3600) / 60);
        } else {
            snprintf(newTimeBuffer, sizeof(newTimeBuffer), "dans %dm",
                remainingSeconds / 60);
        }
        if (strcmp(newTimeBuffer, _lastDisplayedTime) != 0) {
            strncpy(_lastDisplayedTime, newTimeBuffer, sizeof(_lastDisplayedTime));
            shouldRedraw = true;
        }
        _lastUpdateTime = now;
    }

    // Update based on cooling state
    bool coolingNow = (_temperatureController && _temperatureController->isCooling());
    _isIconOn = coolingNow;
    if (_isIconOn != _wasIconOn) {
        DEBUG_PRINT("CoolingController: Icon state changed. Now: ");
        DEBUG_PRINTLN(_isIconOn);
        // Icon state changed, redraw immediately
        if (_view) {
            _view->drawIcons(_isIconOn);
            _view->sendBuffer();
        }
        _wasIconOn = _isIconOn;
    }

    if (difftime(now, _lastUpdateTime) >= 10.0) {
        _temperatureGraph.commitAverage(_currentTemp);
        if(_view) _view->drawGraph(_temperatureGraph);
        shouldRedraw = true;
    }

    auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : InputManager::EncoderDirection::None;
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        shouldRedraw = true;
    }

    if (shouldRedraw) {
        drawScreen();
        if (_view) _view->sendBuffer();
    }
    return true;
}

void CoolingController::drawScreen() {
    if (!_view) return;
    _view->clear();
    const tm* tm_end = localtime(&_endTime);
    char timeBuffer[34] = {'\0'};
    snprintf(timeBuffer, sizeof(timeBuffer), "D\xC3\xA9marrage de la\npousse \xC3\xA0 %d:%02d", tm_end->tm_hour, tm_end->tm_min);
    _view->drawTitle(timeBuffer);

    _view->drawTime(_lastDisplayedTime);
    drawTemperature();
    _view->drawGraph(_temperatureGraph);
    _view->drawButtons("D\xC3\xA9marrer", "Annuler", _onCancelButton ? 1 : 0);
    _view->drawIcons(_isIconOn);
}

void CoolingController::drawTemperature() {
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°", _currentTemp);
    if (_view) _view->drawTemperature(tempBuffer);
}

void CoolingController::prepare(TimeCalculatorCallback callback, Screen* proofingController, Screen* menuScreen) {
    _timeCalculator = callback;
    _proofingController = proofingController;
    _menuScreen = menuScreen;
}
