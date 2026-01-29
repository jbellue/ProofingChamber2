#include "WiFiResetController.h"
#include "../views/WiFiResetView.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

WiFiResetController::WiFiResetController(AppContext* ctx)
    : BaseController(ctx), _networkService(nullptr), _rebootService(nullptr), _view(nullptr), _onCancelButton(true) {}

void WiFiResetController::begin() {
    beginImpl();
}

void WiFiResetController::beginImpl() {
    initializeInputManager();
    AppContext* ctx = getContext();
    if (ctx) {
        if (!_networkService) _networkService = ctx->networkService;
        if (!_rebootService) _rebootService = ctx->rebootService;
        if (!_view) _view = ctx->wifiResetView;
    }
    _onCancelButton = true;
    _view->start();
}

bool WiFiResetController::update(bool forceRedraw) {
    IInputManager* inputManager = getInputManager();
    bool redraw = forceRedraw;
    const auto encoderDirection = inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        redraw = true;
    }
    if (redraw) {
        const char* buttons[] = {"Confirmer", "Annuler"};
        _view->drawButtons(buttons, 2, _onCancelButton ? 1 : 0);
        _view->sendBuffer();
    }
    if (inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        _networkService->resetSettings();
        _view->showResetMessage();
        vTaskDelay(pdMS_TO_TICKS(2000));
        _rebootService->reboot();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    return true;
}
