#include "BaseController.h"
#include "../AppContextDecl.h"

BaseController::BaseController(AppContext* ctx)
    : _ctx(ctx), _inputManager(nullptr), _nextScreen(nullptr) {
}

void BaseController::initializeInputManager() {
    if (!_inputManager && _ctx) {
        _inputManager = _ctx->input;
    }
    _inputManager->resetEncoderPosition();
}
