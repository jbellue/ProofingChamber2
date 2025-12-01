#include "ProofingController.h"
#include "../../DebugUtils.h"
#include "../../icons.h"
#include "../views/ProofingView.h"
#include "../../ITemperatureController.h"
#include "../../SafePtr.h"

ProofingController::ProofingController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _ctx(ctx), _startTime(0),
      _lastTemperatureUpdate(0), _lastGraphUpdate(0), _previousDiffSeconds(0),
      _temperatureController(nullptr)
{}

void ProofingController::beginImpl() {
    _inputManager = SafePtr::resolve(_ctx->input);
    _temperatureController = SafePtr::resolve(_ctx->tempController);
    _view = _ctx->proofingView;
    struct tm startTime;
    getLocalTime(&startTime);
    _startTime = mktime(&startTime);
    _inputManager->slowTemperaturePolling(false);
    _previousDiffSeconds = -60; // Force a redraw on the first update

    _temperatureController->setMode(ITemperatureController::HEATING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _view->start(_inputManager->getTemperature(), _temperatureGraph);
}

bool ProofingController::update(bool shouldRedraw) {
    if (_inputManager->isButtonPressed()) {
        _inputManager->slowTemperaturePolling(true);
        _temperatureController->setMode(ITemperatureController::OFF);
        _view->reset();
        return false;
    }

    struct tm now;
    getLocalTime(&now);
    const time_t now_time = mktime(&now);

    if (difftime(now_time, _lastTemperatureUpdate) >= 1) {
        _lastTemperatureUpdate = now_time;
        const float currentTemp = _inputManager->getTemperature();
        _temperatureGraph.addValueToAverage(currentTemp);
        shouldRedraw |= _view->drawTemperature(currentTemp);
        _temperatureController->update(currentTemp);

        if (difftime(now_time, _lastGraphUpdate) >= 10) {
            _temperatureGraph.commitAverage(currentTemp);
            _lastGraphUpdate = now_time;
            _view->drawGraph(_temperatureGraph);
            shouldRedraw = true;
        }
    }

    shouldRedraw |= _view->drawIcons(_temperatureController->isHeating() ? IconState::On : IconState::Off);
    shouldRedraw |= _view->drawTime(difftime(now_time, _startTime));

    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}
