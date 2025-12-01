#include "AdjustValueController.h"
#include "../views/AdjustValueView.h"
#include "DebugUtils.h"
#include "icons.h"
#include "SafePtr.h"

AdjustValueController::AdjustValueController(AppContext* ctx) :
    _view(nullptr),
    _inputManager(nullptr),
    _storage(nullptr),
    _ctx(ctx)
{}

void AdjustValueController::prepare(const char* title, const char* path) {
    _title = title;
    _path = path;
}

void AdjustValueController::beginImpl() {
    // Obtain storage from the AppContext at begin time (ctx is populated in setup)
    if (_ctx && _ctx->storage) {
        _storage = _ctx->storage;
    }
    // Late-bind view and input
    if (_ctx) {
        _view = _ctx->adjustValueView;
        if (!_inputManager) _inputManager = SafePtr::resolve(_ctx->input);
    }
    if (_storage) {
        _currentValue = _storage->readInt(_path, 0); // Load initial value
    } else {
        _currentValue = 0;
    }
    _inputManager->resetEncoderPosition();

    _valueY = _view->start(_title, _currentValue);
}

bool AdjustValueController::update(bool shouldRedraw) {
    // Handle encoder button press to confirm and save
    if (_inputManager->isButtonPressed()) {
        DEBUG_PRINTLN("AdjustValue: Button pressed, saving value.");
        if (_storage) {
            _storage->writeInt(_path, _currentValue);
        }
        DEBUG_PRINTLN("AdjustValue: Value saved, exiting screen.");
        return false;
    }
    // Handle encoder rotation
    const auto encoderDirection = _inputManager->getEncoderDirection();
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
