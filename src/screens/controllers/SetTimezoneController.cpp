#include "SetTimezoneController.h"
#include "../views/SetTimezoneView.h"
#include "SafePtr.h"

SetTimezoneController::SetTimezoneController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr) {}


void SetTimezoneController::beginImpl() {
    initializeInputManager();
    AppContext* ctx = getContext();
    if (ctx) {
        if (!_view) {
            _view = ctx->setTimezoneView;
        }
    }
    _view->start();
}

bool SetTimezoneController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    return !(inputManager->isButtonPressed());
}
