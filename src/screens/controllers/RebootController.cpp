#include "RebootController.h"
#include "../views/RebootView.h"
#include "../../DebugUtils.h"

RebootController::RebootController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _rebootService(nullptr), _onCancelButton(true), _ctx(ctx) {}


void RebootController::beginImpl() {
    if (_ctx) {
        _inputManager = _ctx->input;
        _rebootService = _ctx->rebootService;
        _view = _ctx->rebootView;
    }
    _onCancelButton = true;
    if (_inputManager) _inputManager->resetEncoderPosition();
    _view->start();
    _view->sendBuffer();
}

bool RebootController::update(bool shouldRedraw) {
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        _view->sendBuffer();
    }
    if (_inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            return false;
        }
        if (_rebootService) {
            DEBUG_PRINTLN("RebootController: invoking reboot service");
            _rebootService->reboot();
        }
        delay(2000);
    }
    return true;
}

