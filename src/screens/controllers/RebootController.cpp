#include "RebootController.h"
#include "../views/RebootView.h"
#include "../../DebugUtils.h"

RebootController::RebootController(AppContext* ctx)
    : _view(nullptr), _inputManager(nullptr), _rebootService(nullptr), _onCancelButton(true), _ctx(ctx) {}

RebootController::~RebootController() {
    if (_view) {
        delete _view;
        _view = nullptr;
    }
}

void RebootController::beginImpl() {
    if (_ctx) {
        _inputManager = _ctx->input;
        _rebootService = _ctx->rebootService;
        if (!_view && _ctx->display) {
            _view = new RebootView(_ctx->display);
        }
    }
    _onCancelButton = true;
    if (_inputManager) _inputManager->resetEncoderPosition();
    if (_view) _view->showTitle();
}

bool RebootController::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    if (_inputManager) {
        const auto encoderDirection = _inputManager->getEncoderDirection();
        if (encoderDirection != InputManager::EncoderDirection::None) {
            _onCancelButton = !_onCancelButton;
            redraw = true;
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
    }
    if (redraw) {
        drawScreen();
    }
    return true;
}

void RebootController::drawScreen() {
    if (_view) _view->drawButtons("Confirmer", "Annuler", _onCancelButton ? 1 : 0);
}
