#include "AdjustValueController.h"
#include "../views/AdjustValueView.h"
#include "DebugUtils.h"
#include "icons.h"

AdjustValueController::AdjustValueController(AppContext* ctx) :
    BaseController(ctx),
    _view(nullptr),
    _storage(nullptr)
{}

void AdjustValueController::prepare(const char* title, const char* path) {
    _title = title;
    _path = path;
}

void AdjustValueController::beginImpl() {
    initializeInputManager();
    
    AppContext* ctx = getContext();
    if (ctx) {
        _storage = ctx->storage;
        _view = ctx->adjustValueView;
    }
    
    if (_storage) {
        _currentValue = _storage->getInt(_path, 0);
    } else {
        _currentValue = 0;
    }

    _valueY = _view->start(_title, _currentValue);
}

bool AdjustValueController::update(bool shouldRedraw) {
    IInputManager* inputManager = getInputManager();
    // Handle encoder button press to confirm and save
    if (inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        if (_storage) {
            _storage->setInt(_path, _currentValue);
        }
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    // Handle encoder rotation
    const auto encoderDirection = inputManager->getEncoderDirection();
    if (shouldRedraw || encoderDirection != IInputManager::EncoderDirection::None) {
        if (encoderDirection == IInputManager::EncoderDirection::Clockwise) {
            _currentValue += 1;
        } else if (encoderDirection == IInputManager::EncoderDirection::CounterClockwise) {
            _currentValue -= 1;
        }
        shouldRedraw |= _view->drawValue(_currentValue, _valueY);
    }
    if (shouldRedraw) {
        _view->sendBuffer();
    }
    return true;
}
