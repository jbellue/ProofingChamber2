#include "CoolingController.h"
#include "DebugUtils.h"

CoolingController::CoolingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _temperatureController(nullptr), _ctx(ctx), _endTime(0), _lastUpdateTime(0), _previousTemp(200.0f), _onCancelButton(true), _proofingController(nullptr), _menuScreen(nullptr) {}

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
    if (difftime(now, _lastUpdateTime) >= 1) {
        const float currentTemp = _inputManager ? _inputManager->getTemperature() : 0.0f;
        DEBUG_PRINT("Current temperature: ");
        DEBUG_PRINT(currentTemp);
        DEBUG_PRINT(" - Previous: ");
        DEBUG_PRINTLN(_previousTemp);
        _lastUpdateTime = now;
        if (abs(currentTemp - _previousTemp) > 0.1) {
            _previousTemp = currentTemp;
            if (_temperatureController) _temperatureController->update(currentTemp);
        }
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

    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);
    const int remainingSeconds = difftime(_endTime, now);
    if (remainingSeconds >= 3600) {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %dh%02dm)",
            remainingSeconds / 3600, (remainingSeconds % 3600) / 60);
    } else if (remainingSeconds >= 60) {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %dm%02ds)",
            remainingSeconds / 60, remainingSeconds % 60);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "(dans %ds)", remainingSeconds);
    }
    drawTime(timeBuffer);
    _view->drawButtons("D\xC3\xA9marrer", "Annuler", _onCancelButton ? 1 : 0);
}

void CoolingController::drawTime(const char* timeBuffer) {
    if (_view) _view->drawTime(timeBuffer);
}


void CoolingController::prepare(TimeCalculatorCallback callback, Screen* proofingController, Screen* menuScreen) {
    _timeCalculator = callback;
    _proofingController = proofingController;
    _menuScreen = menuScreen;
}
