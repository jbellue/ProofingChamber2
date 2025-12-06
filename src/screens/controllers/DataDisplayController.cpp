#include "DataDisplayController.h"
#include "../views/DataDisplayView.h"
#include "DebugUtils.h"
#include "../../SafePtr.h"

void DataDisplayController::beginImpl() {
    DEBUG_PRINTLN("DataDisplayController::beginImpl called");
    initializeInputManager();

    AppContext* ctx = getContext();
    _view = ctx->dataDisplayView;
    _view->start();
    getInputManager()->slowTemperaturePolling(false);
}

bool DataDisplayController::update(bool forceRedraw) {
    IInputManager* input = getInputManager();

    // Return to previous screen on button press
    if (input->isButtonPressed()) {
        return false;
    }

    struct tm tm_now;
    getLocalTime(&tm_now);
    const time_t now = mktime(&tm_now);
    if (difftime(now, _lastUpdateTime) >= 1) {
        const float currentTemp = input->getTemperature();
        _lastUpdateTime = now;
        forceRedraw |= _view->drawTemperature(currentTemp);
        forceRedraw |= _view->drawTime(tm_now);
    }
    if (forceRedraw) {
        _view->sendBuffer();
    }
    return true;
}