#include "SetTimezoneController.h"

SetTimezoneController::SetTimezoneController(AppContext* ctx)
    : _ctx(ctx), _inputManager(nullptr), _view(nullptr) {}

void SetTimezoneController::begin() {
    beginImpl();
}

void SetTimezoneController::beginImpl() {
    if (_ctx) {
        if (!_inputManager) _inputManager = _ctx->input;
        if (!_view) _view = new SetTimezoneView(_ctx->display);
    }
    if (_inputManager) _inputManager->resetEncoderPosition();
    if (_view) _view->showInitialPrompt();
}

bool SetTimezoneController::update(bool forceRedraw) {
    bool redraw = forceRedraw;
    if (redraw && _view) {
        _view->drawButton("OK", true);
        _view->sendBuffer();
    }
    return !(_inputManager && _inputManager->isButtonPressed());
}
