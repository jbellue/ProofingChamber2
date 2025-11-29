#include "WiFiResetController.h"
#include "../views/WiFiResetView.h"

WiFiResetController::WiFiResetController(AppContext* ctx)
    : _ctx(ctx), _inputManager(nullptr), _networkService(nullptr), _rebootService(nullptr), _view(nullptr), _onCancelButton(true) {}

void WiFiResetController::begin() {
    beginImpl();
}

void WiFiResetController::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_networkService) _networkService = _ctx->networkService;
        if (!_rebootService) _rebootService = _ctx->rebootService;
        if (!_view) {
            _view = _ctx->wifiResetView;
        }
    }
    _onCancelButton = true;
    if (_inputManager) _inputManager->resetEncoderPosition();
    _view->showInitialPrompt();
}

bool WiFiResetController::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    const auto encoderDirection = _inputManager ? _inputManager->getEncoderDirection() : InputManager::EncoderDirection::None;
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        redraw = true;
    }
    if (redraw) {
        _view->drawButtons("Confirmer", "Annuler", _onCancelButton ? 1 : 0);
        _view->sendBuffer();
    }
    if (_inputManager && _inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        if (_networkService) {
            _networkService->resetSettings();
        }
        _view->showResetMessage();
        delay(2000);
        if (_rebootService) {
            _rebootService->reboot();
        }
        delay(2000);
    }
    return true;
}
