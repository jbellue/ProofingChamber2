#include "ProofingController.h"
#include "../../DebugUtils.h"
#include "../../icons.h"
#include "../views/ProofingView.h"
#include "../../ITemperatureController.h"
#include "../../ScreensManager.h"
#include "../Menu.h"

ProofingController::ProofingController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr), _startTime(0),
      _lastTemperatureUpdate(0), _lastGraphUpdate(0), _previousDiffSeconds(0),
      _temperatureController(nullptr)
{}

void ProofingController::beginImpl() {
    initializeInputManager();
    
    AppContext* ctx = getContext();
    _temperatureController = ctx->tempController;
    _view = ctx->proofingView;
    
    // Only set start time if not already set (from web API)
    if (_startTime == 0) {
        struct tm startTime;
        getLocalTime(&startTime);
        _startTime = mktime(&startTime);
    }
    
    getInputManager()->slowTemperaturePolling(false);
    _previousDiffSeconds = -60; // Force a redraw on the first update

    // Only set heating mode if not already set (from web API)
    if (_temperatureController->getMode() != ITemperatureController::HEATING) {
        _temperatureController->setMode(ITemperatureController::HEATING);
    }
    
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    _view->start(getInputManager()->getTemperature(), _temperatureGraph);
}

bool ProofingController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    if (inputManager->isButtonPressed()) {
        inputManager->slowTemperaturePolling(true);
        _temperatureController->setMode(ITemperatureController::OFF);
        _view->reset();
        _startTime = 0; // Reset when exiting
        return false;
    }

    struct tm now;
    getLocalTime(&now);
    const time_t now_time = mktime(&now);

    if (difftime(now_time, _lastTemperatureUpdate) >= 1) {
        _lastTemperatureUpdate = now_time;
        const float currentTemp = inputManager->getTemperature();
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

    shouldRedraw |= _view->drawIcons(OptionalBool(_temperatureController->isHeating()));
    shouldRedraw |= _view->drawTime(difftime(now_time, _startTime));

    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}

void ProofingController::startProofing() {
    AppContext* ctx = getContext();
    if (!ctx || !ctx->tempController) return;
    
    _temperatureController = ctx->tempController;
    
    struct tm startTime;
    getLocalTime(&startTime);
    _startTime = mktime(&startTime);
    _lastTemperatureUpdate = 0;
    _lastGraphUpdate = 0;
    
    _temperatureController->setMode(ITemperatureController::HEATING);
    _temperatureGraph.configure(30, 15, -5.0, 60.0, true);
    
    DEBUG_PRINTLN("[ProofingController] Started proofing from web API");
    
    // Navigate display to show proofing screen
    if (ctx->screens) {
        ctx->screens->setActiveScreen(this);
        begin(); // Initialize the screen properly
        // Force display refresh
        if (ctx->display) {
            ctx->display->clearBuffer();
            update(true); // Force redraw
            ctx->display->sendBuffer();
        }
    }
    
    // Notify web view of state change (web is a VIEW)
    if (ctx->webServer) {
        ctx->webServer->notifyStateChange();
    }
}

void ProofingController::stopProofing() {
    AppContext* ctx = getContext();
    if (_temperatureController) {
        _temperatureController->setMode(ITemperatureController::OFF);
    }
    _startTime = 0;
    
    DEBUG_PRINTLN("[ProofingController] Stopped proofing from web API");
    
    // Navigate back to menu
    if (ctx && ctx->screens && ctx->menu) {
        ctx->screens->setActiveScreen(ctx->menu);
        ctx->menu->begin(); // Initialize menu to prevent blank screen
        // Force display refresh
        if (ctx->display) {
            ctx->display->clearBuffer();
            ctx->menu->update(true); // Force redraw
            ctx->display->sendBuffer();
        }
    }
    
    // Notify web view of state change (web is a VIEW)
    if (ctx->webServer) {
        ctx->webServer->notifyStateChange();
    }
}
