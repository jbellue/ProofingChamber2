#include "RebootController.h"
#include "../views/RebootView.h"
#include "../../DebugUtils.h"
#include "SafePtr.h"

RebootController::RebootController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _rebootService(nullptr), _onCancelButton(true), _ctx(ctx) {}


void RebootController::beginImpl() {
    if (_ctx) {
        _inputManager = SafePtr::resolve(_ctx->input);
        _rebootService = _ctx->rebootService;
        _view = _ctx->rebootView;
    }
    _onCancelButton = true;
    _inputManager->resetEncoderPosition();
    _view->start();
    _view->sendBuffer();
}

bool RebootController::update(bool shouldRedraw) {
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
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

