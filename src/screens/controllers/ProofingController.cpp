#include "ProofingController.h"
#include "../../DebugUtils.h"
#include "../../icons.h"
#include "../views/ProofingView.h"
#include "../../TemperatureController.h"

ProofingController::ProofingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _ctx(ctx), _startTime(0),
      _lastTemperatureUpdate(0), _lastGraphUpdate(0), _previousDiffSeconds(0), _previousTemp(200.0),
      _currentTemp(0.0), _isIconOn(true), _wasIconOn(false), _temperatureController(nullptr)
{}

void ProofingController::beginImpl() {
    _inputManager = _ctx->input;
    _temperatureController = _ctx->tempController;
    _view = _ctx->proofingView;
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
    _view->clear();
    _view->drawTitle("En pousse depuis");
    _view->drawButtons();
}

bool ProofingController::update(bool shouldRedraw) {
    if (_inputManager->isButtonPressed()) {
        _inputManager->slowTemperaturePolling(true);
        if (_temperatureController) _temperatureController->setMode(TemperatureController::OFF);
        return false;
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
            _view->drawTemperature(_currentTemp);
            shouldRedraw = true;
            if (_temperatureController) _temperatureController->update(_currentTemp);
        }
    }

    // Update based on heating state
    bool heatingNow = (_temperatureController && _temperatureController->isHeating());
    _isIconOn = heatingNow;
    if (_isIconOn != _wasIconOn) {
        // Icon state changed, redraw immediately
        _view->drawIcons(_isIconOn);
        _view->sendBuffer();
        _wasIconOn = _isIconOn;
    }

    if (difftime(now_time, _lastGraphUpdate) >= 10.0) {
        _temperatureGraph.commitAverage(_currentTemp);
        _lastGraphUpdate = now_time;
        _view->drawGraph(_temperatureGraph);
        shouldRedraw = true;
    }

    if (diff_seconds - _previousDiffSeconds >= 60) {
        _previousDiffSeconds = diff_seconds;
        _view->drawTime(_previousDiffSeconds);
        shouldRedraw = true;
    }

    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}
