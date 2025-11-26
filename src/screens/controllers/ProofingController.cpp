#include "ProofingController.h"
#include "../../DebugUtils.h"
#include "../../icons.h"
#include "../views/ProofingView.h"
#include "../../TemperatureController.h"

ProofingController::ProofingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _ctx(ctx), _startTime(0),
      _lastTemperatureUpdate(0), _lastGraphUpdate(0), _previousDiffSeconds(0), _previousTemp(200.0),
      _currentTemp(0.0), _isIconOn(true), _wasIconOn(false), _temperatureController(nullptr)
{
}

ProofingController::~ProofingController() {
    if (_view) {
        delete _view;
        _view = nullptr;
    }
}
void ProofingController::beginImpl() {
    // Late-bind context pointers. `appContext` should be populated in setup().
    if (_ctx) {
        _inputManager = _ctx->input;
        _temperatureController = _ctx->tempController;
        if (!_view && _ctx->display) {
            _view = new ProofingView(_ctx->display);
        }
    }
    struct tm startTime;
    getLocalTime(&startTime);
    _startTime = mktime(&startTime);
    if (_inputManager) _inputManager->slowTemperaturePolling(false);
    _currentTemp = _inputManager->getTemperature();
    _previousTemp = 200.0; // Initialize to a high value to ensure the first update is drawn
    _previousDiffSeconds = -60; // Force a redraw on the first update
    _lastGraphUpdate = 0;       // Force a redraw on the first update
    _lastTemperatureUpdate = 0; // Force a redraw on the first update

    if (_temperatureController) _temperatureController->setMode(TemperatureController::HEATING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    if (_view) {
        _view->clear();
        _view->drawTitle("En pousse depuis");
        _view->drawButtons();
    }
}

bool ProofingController::update(bool shouldRedraw) {
    if (_inputManager) {
        if (_inputManager->isButtonPressed()) {
            _inputManager->slowTemperaturePolling(true);
            if (_temperatureController) _temperatureController->setMode(TemperatureController::OFF);
            return false;
        }
    }

    struct tm now;
    getLocalTime(&now);
    const time_t now_time = mktime(&now);
    const double diff_seconds = difftime(now_time, _startTime);

    if (_inputManager && difftime(now_time, _lastTemperatureUpdate) >= 1) {
        _currentTemp = _inputManager->getTemperature();
        if (abs(_currentTemp - _previousTemp) > 0.1) {
            _temperatureGraph.addValueToAverage(_currentTemp);
            _previousTemp = _currentTemp;
            drawTemperature();
            shouldRedraw = true;
            if (_temperatureController) _temperatureController->update(_currentTemp);
        }
    }

    // Update based on heating state
    bool heatingNow = (_temperatureController && _temperatureController->isHeating());
    _isIconOn = heatingNow;
    if (_isIconOn != _wasIconOn) {
        // Icon state changed, redraw immediately
        if (_view) {
            _view->drawIcons(_isIconOn);
            _view->sendBuffer();
        }
        _wasIconOn = _isIconOn;
    }

    if (difftime(now_time, _lastGraphUpdate) >= 10.0) {
        _temperatureGraph.commitAverage(_currentTemp);
        _lastGraphUpdate = now_time;
        if (_view) {
            _view->drawGraph(_temperatureGraph);
        }
        shouldRedraw = true;
    }

    if (diff_seconds - _previousDiffSeconds >= 60) {
        _previousDiffSeconds = diff_seconds;
        drawTime();
        shouldRedraw = true;
    }

    if (shouldRedraw && _view) {
        _view->sendBuffer();
    }
    return true;
}

void ProofingController::drawTime() {
    const int total_minutes = _previousDiffSeconds / 60;
    const int hours = total_minutes / 60;
    const int minutes = total_minutes % 60;

    char timeBuffer[8] = {'\0'};
    if (hours > 0) {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dh%02dm", hours, minutes);
    } else {
        snprintf(timeBuffer, sizeof(timeBuffer), "%dm", minutes);
    }
    DEBUG_PRINTLN(timeBuffer);
    if (_view) _view->drawTime(timeBuffer);
}

void ProofingController::drawTemperature() {
    char tempBuffer[7] = {'\0'};
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1f°", _currentTemp);
    if (_view) _view->drawTemperature(tempBuffer);
}
