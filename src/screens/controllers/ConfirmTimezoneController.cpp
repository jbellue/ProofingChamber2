#include "ConfirmTimezoneController.h"
#include "../views/ConfirmTimezoneView.h"
#include "../../DebugUtils.h"
#include "../../services/IStorage.h"
#include "SafePtr.h"
#include "../../screens/Menu.h"
#include "../../MenuItems.h"

ConfirmTimezoneController::ConfirmTimezoneController(AppContext* ctx)
    : BaseController(ctx), _view(nullptr) {}

void ConfirmTimezoneController::setTimezoneInfo(const char* continent, const char* tzName, const char* posixString) {
    _timezoneContinentName = continent;
    _timezoneDisplayName = tzName;
    _timezonePosixString = posixString;
}

void ConfirmTimezoneController::beginImpl() {
    initializeInputManager();
    AppContext* ctx = getContext();
    if (ctx) {
        _view = ctx->confirmTimezoneView;
    }
    _onCancelButton = true;
    
    if (_view && _timezoneDisplayName) {
        _view->setTimezone(_timezoneContinentName, _timezoneDisplayName);
        _view->start();
        _view->sendBuffer();
    }
}

bool ConfirmTimezoneController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    const auto encoderDirection = inputManager->getEncoderDirection();
    if (encoderDirection != IInputManager::EncoderDirection::None) {
        _onCancelButton = !_onCancelButton;
        _view->drawButtons(_onCancelButton);
        _view->sendBuffer();
    }
    if (inputManager->isButtonPressed()) {
        if (_onCancelButton) {
            // Return to previous menu
            return false;
        }
        // Confirm timezone selection
        AppContext* ctx = getContext();
        if (ctx && ctx->storage && _timezonePosixString) {
            ctx->storage->writeString("/timezone.txt", _timezonePosixString);
            DEBUG_PRINT("Timezone confirmed and saved: ");
            DEBUG_PRINTLN(_timezonePosixString);
        }
        // Return to Advanced settings menu
        BaseController* next = getNextScreen();
        Menu* menu = static_cast<Menu*>(next);
        if (menu) {
            menu->setCurrentMenu(moreSettingsMenu);
        }
        // Finish this screen to trigger transition
        // TODO apply the timezone change immediately
        return false;
    }
    return true;
}
