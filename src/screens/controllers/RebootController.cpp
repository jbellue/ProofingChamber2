#include "RebootController.h"
#include "../views/RebootView.h"
#include "../../DebugUtils.h"

RebootController::RebootController(AppContext* ctx)
    : _view(new RebootView(ctx->display)), _inputManager(ctx->input), _rebootService(ctx->rebootService), _onCancelButton(true), _ctx(ctx) {}

RebootController::~RebootController() {
    delete _view;
}
void RebootController::beginImpl() {
    _onCancelButton = true;
    _inputManager->resetEncoderPosition();
    _view->showTitle();
}

bool RebootController::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    const auto encoderDirection = _inputManager->getEncoderDirection();
    if (encoderDirection != InputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        redraw = true;
    }
    if (redraw) {
        drawScreen();
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

void RebootController::drawScreen() {
    _view->drawButtons(_onCancelButton);
}
