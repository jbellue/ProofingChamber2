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

bool SetTimezoneController::update(bool shouldRedraw) {
    if (shouldRedraw) {
        const char* buttons[] = {"OK"};
        _view->drawButtons(buttons, 1, 0);
        _view->sendBuffer();
    }
    return !(_inputManager && _inputManager->isButtonPressed());
}
