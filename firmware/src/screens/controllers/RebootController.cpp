#include "RebootController.h"
#include "../views/RebootView.h"
#include "../../DebugUtils.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

RebootController::RebootController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr), _rebootService(nullptr), _onCancelButton(true) {}


void RebootController::beginImpl() {
    initializeInputManager();
    AppContext* ctx = getContext();
    if (ctx) {
        _rebootService = ctx->rebootService;
        _view = ctx->rebootView;
    }
    _onCancelButton = true;
    _view->start();
    _view->sendBuffer();
}

bool RebootController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    const auto encoderDirection = inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        _view->sendBuffer();
    }
    if (inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        if (_rebootService) {
            DEBUG_PRINTLN("RebootController: invoking reboot service");
            _rebootService->reboot();
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    return true;
}

