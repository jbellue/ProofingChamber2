#include "SetTimezoneController.h"
#include "../views/SetTimezoneView.h"
#include <new>

SetTimezoneController::SetTimezoneController(AppContext* ctx)
    : _ctx(ctx), _inputManager(nullptr), _view(nullptr) {}


void SetTimezoneController::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_view) {
            _view = _ctx->setTimezoneView;
        }
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    _view->showInitialPrompt();
}

bool SetTimezoneController::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    if (redraw) {
        _view->drawButton("OK", true);
        _view->sendBuffer();
    }
    return !(_inputManager && _inputManager->isButtonPressed());
}
